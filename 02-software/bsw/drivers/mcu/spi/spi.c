/*
 * spi.c
 *
 * Created on: Jun 25, 2021
 * Author: Stephane Amans
 *
 *    Change log:
 *
 *    02/08/2022: Refactor driver
 *
 */

/* Include files        */
#include <stdio.h>
#include <string.h>

#include "fault.h"
#include "clock.h"
#include "spi.h"

#include "regbase_spi.h"

#include "soc_configuration.h"

/* Defines */
#define SPI_CR1_CPHA_BIT_MASK       0x1
#define SPI_CR1_CPOL_BIT_MASK       0x2
#define SPI_CR1_MSTR_BIT_MASK       0x4
#define SPI_CR1_BR_BIT_MASK         0x38
#define SPI_CR1_SPE_BIT_MASK        0x40
#define SPI_CR1_LSBFIRST_BIT_MASK   0x80
#define SPI_CR1_SSI_BIT_MASK        0x100
#define SPI_CR1_SSE_BIT_MASK        0x200
#define SPI_CR1_DFF_BIT_MASK        0x800

#define SPI_CR2_RXDMAEN_BIT_MASK    0x1
#define SPI_CR2_TXDMAEN_BIT_MASK    0x2
#define SPI_CR2_SSOE_BIT_MASK       0x4
#define SPI_CR2_RXNEIE_BIT_MASK     0x40
#define SPI_CR2_TXEIE_BIT_MASK      0x80

#define SPI_SR_TXE_BIT_MASK         0x2
#define SPI_SR_RXNE_BIT_MASK        0x1

#define SPI_SR_UDR_BIT_MASK         0x8
#define SPI_SR_CRCERR_BIT_MASK      0x10
#define SPI_SR_MODF_BIT_MASK        0x20
#define SPI_SR_OVR_BIT_MASK         0x40

/* SPI private structure definition :        */
struct t_spi_private
{
    struct t_spi_regs *reg;
    struct t_spi_slave *slave;
    uint32_t clock_frequency;
    uint16_t freq_khz;
    uint8_t last_config;
    uint16_t *write_buffer;
    uint16_t *read_buffer;
    uint32_t buffer_length;
    uint16_t buffer_index;
    struct t_dma_client spi_dma_tx;
    struct t_dma_client spi_dma_rx;
    struct
    {
        void (*transfer)(struct t_spi_driver *driver, struct t_spi_slave *slave);
    }methods;
};

/* Static SPI private and slaves record structure instances. */
static struct t_spi_private priv[SPI_IP_NUMBER];
static struct t_spi_slave *slaves_record[MAX_SPI1_PERIPHERALS + MAX_SPI2_PERIPHERALS];

/** Chip select pin control low state.
 *
 * \param slave: Pointer to the slave parameters.
 *
 * \return void;
 *
 */
static void cs_low(struct t_spi_slave *slave)
{
    gpio_write(slave->cs, false);
};

/** Chip select pin control high state.
 *
 * \param slave: Pointer to the slave parameters.
 *
 * \return void;
 *
 */
static void cs_high(struct t_spi_slave *slave)
{
     gpio_write(slave->cs, true);
};

/** Compute frequency divider code.
 *
 * \param bus_frequency: Internal bus frequency.
 * \param spi_frequency: Expected SPI frequency.
 * \param divider_code: Pointer to the divider code computed location.
 *
 * \return : Error code or ERROR_OK.
 *
 */
static t_error_handling compute_frequency_divider(uint32_t bus_frequency, uint16_t spi_frequency, uint8_t *divider_code)
{
    t_error_handling error;
    uint32_t local_spi_frequency = (uint32_t)spi_frequency * 1000;
    uint16_t divider = (uint16_t)(bus_frequency/local_spi_frequency);
    uint32_t modulo = bus_frequency%local_spi_frequency;

    if(divider < 3)
    {
        *divider_code = 0;
    }
    else if((divider >= 3) && (divider < 6))
    {
        *divider_code = (1 << 3) & SPI_CR1_BR_BIT_MASK;
    }
    else if((divider >= 6) && (divider < 12))
    {
        *divider_code = (2 << 3) & SPI_CR1_BR_BIT_MASK;
    }
    else if((divider >= 12) && (divider < 20))
    {
        *divider_code = (3 << 3) & SPI_CR1_BR_BIT_MASK;
    }
    else if((divider >= 20) && (divider < 48))
    {
        *divider_code = (4 << 3) & SPI_CR1_BR_BIT_MASK;
    }
    else if((divider >= 48) && (divider < 96))
    {
        *divider_code = (5 << 3) & SPI_CR1_BR_BIT_MASK;
    }
    else if((divider >= 96) && (divider < 192))
    {
        *divider_code = (6 << 3) & SPI_CR1_BR_BIT_MASK;
    }
    else
    {
        *divider_code = (7 << 3) & SPI_CR1_BR_BIT_MASK;
    }

    if(modulo != 0)
    {
        error = ERROR_WRONG_CLOCK_SET;
    }
    else
    {
        error = ERROR_OK;
    }
    return error;
};

/** Start a SPI transfer by DMA.
 *
 * \param driver: Pointer to the SPI driver.
 * \param slave: Pointer to the slave instance.
 *
 * \return : void.
 *
 */
static void spi_transfer_dma (struct t_spi_driver *driver, struct t_spi_slave *slave)
{
    driver->priv->slave = slave;
    if(slave->data_length == spi_data_8_bits)
    {
        driver->priv->spi_dma_tx.mem_data_type = dma_8_bits;
    }

    else
    {
        driver->priv->spi_dma_tx.mem_data_type = dma_16_bits;
    }

    /* Save transfer parameters. */
    driver->priv->spi_dma_rx.mem_data_type = dma_16_bits;

    driver->priv->spi_dma_tx.transfer_length = driver->priv->buffer_length;
    driver->priv->spi_dma_rx.transfer_length = driver->priv->buffer_length;

    driver->priv->spi_dma_tx.memory_address = (uintptr_t)driver->priv->write_buffer;
    driver->priv->spi_dma_rx.memory_address = (uintptr_t)driver->priv->read_buffer;

    driver->priv->spi_dma_tx.peripheral_address = (uintptr_t)&driver->priv->reg->DR;
    driver->priv->spi_dma_rx.peripheral_address = (uintptr_t)&driver->priv->reg->DR;

    /* Set DMA channels transfer parameters */
    dma_set_transfer(driver->dma.tx_channel, &driver->priv->spi_dma_tx);
    dma_set_transfer(driver->dma.rx_channel, &driver->priv->spi_dma_rx);

    /* Activate DMA transfers */
    dma_start_transfer(driver->dma.tx_channel);
    dma_start_transfer(driver->dma.rx_channel);
}

/** Start a SPI transfer by interrupt.
 *
 * \param driver: Pointer to the SPI driver.
 * \param slave: Pointer to the slave instance.
 *
 * \return : void.
 *
 */
static void spi_transfer_irq(struct t_spi_driver *driver, struct t_spi_slave *slave)
{
    driver->priv->buffer_index = 0;
    driver->priv->slave = slave;

    /* Load buffer with data */
    driver->priv->reg->DR = driver->priv->write_buffer[0];

    /* Enable TX buffer empty and RX buffer not empty interrupts */
    driver->priv->reg->CR2 |= SPI_CR2_TXEIE_BIT_MASK | SPI_CR2_RXNEIE_BIT_MASK;
}

/** Start a SPI transfer by polling.
 *
 * \param driver: Pointer to the SPI driver.
 * \param slave: Pointer to the slave instance.
 * \param data: Pointer to the data structure
 *
 * \return : void.
 *
 */
static void spi_transfer_poll(struct t_spi_driver *driver, struct t_spi_slave *slave)
{
    driver->priv->buffer_index = 0;

    while(driver->priv->buffer_length > 0)
    {
        driver->priv->reg->DR = driver->priv->write_buffer[driver->priv->buffer_index];
        while((driver->priv->reg->SR & SPI_SR_TXE_BIT_MASK) != SPI_SR_TXE_BIT_MASK){}
        while((driver->priv->reg->SR & SPI_SR_RXNE_BIT_MASK) != SPI_SR_RXNE_BIT_MASK){}
        driver->priv->read_buffer[driver->priv->buffer_index] = driver->priv->reg->DR;
        driver->priv->buffer_index++;
        driver->priv->buffer_length--;
    };
    cs_high(slave);
}

/** Update the peripheral configuration.
 *
 * \param driver: Pointer to the SPI driver.
 * \param slave: Pointer to the slave configuration.
 *
 * \return : Error code or ERROR_OK.
 *
 */
static t_error_handling update_spi_configuration(const struct t_spi_driver *driver, struct t_spi_slave *slave)
{
    t_error_handling error = ERROR_OK;

    if(slave->freq_khz != driver->priv->freq_khz)
    {
        uint16_t local_mask;
        if(compute_frequency_divider(driver->priv->clock_frequency,
                                     slave->freq_khz,
                                     (uint8_t*)&local_mask) == ERROR_OK)
        {
            driver->priv->reg->CR1 &= ~SPI_CR1_BR_BIT_MASK;
            driver->priv->reg->CR1 |= local_mask;
            driver->priv->freq_khz = slave->freq_khz;
        }
        else
        {
            error = ERROR_WRONG_CLOCK_SET;
        }
    }

    if(slave->clock_phase == spi_clk_first)
    {
        driver->priv->reg->CR1 &= ~SPI_CR1_CPHA_BIT_MASK;
    }
    else
    {
        driver->priv->reg->CR1 |= SPI_CR1_CPHA_BIT_MASK;
    }

    if(slave->clock_polarity == spi_clk_rising)
    {
        driver->priv->reg->CR1 &= ~SPI_CR1_CPOL_BIT_MASK;
    }
    else
    {
        driver->priv->reg->CR1 |= SPI_CR1_CPOL_BIT_MASK;
    }

    if(slave->frame_direction == spi_frame_msb_first)
    {
        driver->priv->reg->CR1 &= ~SPI_CR1_LSBFIRST_BIT_MASK;
    }
    else
    {
        driver->priv->reg->CR1 |= SPI_CR1_LSBFIRST_BIT_MASK;
    }

    if(slave->data_length == spi_data_8_bits)
    {
        driver->priv->reg->CR1 &= ~SPI_CR1_DFF_BIT_MASK;
    }
    else
    {
        driver->priv->reg->CR1 |= SPI_CR1_DFF_BIT_MASK;
    }
    return error;
};

void spi_dma_ch2_irq_management(struct t_dma_status *dma_status)
{
    if(dma_status->transfer_complete == true)
    {
        cs_high(priv[0].slave);
    }
}

void spi_slave_register(struct t_spi_slave *slave)
{
    uint32_t idx_pos = 0;
    uint32_t idx_scan = 0;

    /* Scan the slave peripherals table to find a free location */
    while((slaves_record[idx_pos] != 0) &&
          (idx_pos < (MAX_SPI1_PERIPHERALS + MAX_SPI2_PERIPHERALS)))
    {
        idx_pos++;
    };
    
    /* Record the peripheral if it is the first */
    if(idx_pos == 0)
    {
        slaves_record[idx_pos] = slave;
        slaves_record[idx_pos]->id = idx_pos;
    }
    else
    {
        /* Record the new peripheral parameters in the free location */
        while(idx_scan <= idx_pos)
        {
            if((slaves_record[idx_scan]->freq_khz != slave->freq_khz) ||
               (slaves_record[idx_scan]->data_length != slave->data_length) ||
               (slaves_record[idx_scan]->frame_direction != slave->frame_direction) ||
               (slaves_record[idx_scan]->clock_phase != slave->clock_phase) ||
               (slaves_record[idx_scan]->clock_polarity != slave->clock_polarity))
            {
                slaves_record[idx_pos] = slave;
                slaves_record[idx_pos]->id = idx_pos;
                break;
            }
            else
            {
                idx_scan++;
            }
        }
    }
}

t_error_handling spi_transfer(struct t_spi_driver *driver, struct t_spi_slave *slave, struct t_spi_data *data)
{
    uint8_t index = 0;
    t_error_handling error = ERROR_OK;

    /* Save transfer's SPI data parameters */
    driver->priv->write_buffer = data->write_buffer;
    driver->priv->read_buffer = data->read_buffer;
    driver->priv->buffer_length = data->length;

    /* If the slave is not the same, change slave configuration, the peripheral must be registered in the local base */
    if(slave->id != driver->priv->last_config)
    {
        while((slave->id != slaves_record[index]->id) && (index < (MAX_SPI1_PERIPHERALS + MAX_SPI2_PERIPHERALS)))
        {
            index++;
        };

        if(index >= (MAX_SPI1_PERIPHERALS + MAX_SPI2_PERIPHERALS))
        {
            error = ERROR_SPI_PERIPH_UNKNOWN;
        }

        else
        {
            update_spi_configuration(driver, slave);
            driver->priv->last_config = slave->id;
        }
    }

    /* IF the CS pin is high (no transfer started yet), then perform the new transfer request. */
    if(gpio_read(slave->cs) == true)
    {
        error = ERROR_BUSY;
        cs_low(slave);

        driver->priv->methods.transfer(driver, slave);
    }
    return error;
}

t_error_handling spi_transfer_status(struct t_spi_driver *driver, struct t_spi_slave *slave)
{
    t_error_handling error;

    if(gpio_read(slave->cs) == false)
    {
        error = ERROR_BUSY;
    }
    else
    {
        /* If the CS pin is high (transfer ended), check if any error occured */
        if((driver->priv->reg->SR & SPI_SR_UDR_BIT_MASK) == SPI_SR_UDR_BIT_MASK)
        {
            error = ERROR_SPI_UNDERUN;
        }
        else if((driver->priv->reg->SR & SPI_SR_CRCERR_BIT_MASK) == SPI_SR_CRCERR_BIT_MASK)
        {
            error = ERROR_SPI_CRC;
        }
        else if((driver->priv->reg->SR & SPI_SR_MODF_BIT_MASK) == SPI_SR_MODF_BIT_MASK)
        {
            error = ERROR_SPI_MODE;
        }
        else if((driver->priv->reg->SR & SPI_SR_OVR_BIT_MASK) == SPI_SR_OVR_BIT_MASK)
        {
            error = ERROR_SPI_OVERUN;
        }
        else if((driver->dma.active == true) &&
               ((dma_get_transfer_error(driver->dma.tx_channel) == true) ||
                (dma_get_transfer_error(driver->dma.rx_channel) == true)))
        {
            error = ERROR_DMA_ERROR;
        }
        else
        {
            error = ERROR_OK;
        }
    }
    return error;
}

void spi_uninitialization(struct t_spi_driver *driver)
{
    /* Reset the module */
    reset_module(driver->peripheral);

    /* Disable periperal clock */
    disable_clock(driver->peripheral);

    /* Clear private structure */
    memset(driver->priv, 0, sizeof(struct t_spi_private));
}   

void spi_initialization(struct t_spi_driver *config)
{
    /* Get the SoC frequency parameters */
    struct t_clock_driver *clock_driver = get_clock_driver();

    /* Associate private instance to the driver */
    config->priv = &priv[config->instance];

    /* Clear the private structure instance */
    memset(config->priv, 0, sizeof(struct t_spi_private));

    /* Locate registers to the base address */
    config->priv->reg = (struct t_spi_regs*)config->base_address;

    /* Clear the record table. */
    memset(slaves_record, 0,
           sizeof(struct t_spi_slave[MAX_SPI1_PERIPHERALS + MAX_SPI2_PERIPHERALS]));

    /* For any SPI instance */
    if(config->instance == 0)
    {
   	    config->priv->clock_frequency = clock_driver->APB2_clk_freq;
    }
    else if(config->instance == 1)
    {
    	config->priv->clock_frequency = clock_driver->APB1_clk_freq;
    }

    enable_clock(config->peripheral);

    /* SPI in master mode */
    config->priv->reg->CR1 |= SPI_CR1_MSTR_BIT_MASK;

    /* NSS pin is not used */
    config->priv->reg->CR1 |= (SPI_CR1_SSI_BIT_MASK | SPI_CR1_SSE_BIT_MASK);

    /* Enable SPI Module */
    config->priv->reg->CR1 |= SPI_CR1_SPE_BIT_MASK;

    /* Initialize last_config to force a configuration initializastion at the first start */
    config->priv->last_config = UINT8_MAX;

    if(config->irq.active == true)
    {
        enable_nvic_irq(config->instance + NVIC_SPI_OFFSET);
        set_nvic_priority(config->instance + NVIC_SPI_OFFSET, config->irq.priority);

        /* Point to the IRQ dedicated transfer method */
        config->priv->methods.transfer = &spi_transfer_irq;
    }

    else if(config->dma.active == true)
    {
        /* Set DMA client parameters */
        config->priv->spi_dma_tx.memory_increment = true;
        config->priv->spi_dma_tx.peripheral_increment = false;
        config->priv->spi_dma_tx.read_from_memory = true;

        config->priv->spi_dma_rx.memory_increment = true;
        config->priv->spi_dma_rx.peripheral_increment = false;
        config->priv->spi_dma_rx.read_from_memory = false;

        /* Enable TX and RX DMA transmission */
        config->priv->reg->CR2 |= SPI_CR2_RXDMAEN_BIT_MASK | SPI_CR2_TXDMAEN_BIT_MASK;

        /* Point to the DMA dedicated transfer method */
        config->priv->methods.transfer = &spi_transfer_dma;
    }

    else
    {
        /* Point to the polling dedicated transfer method */
        config->priv->methods.transfer = &spi_transfer_poll;
    }
}

/** SPI1 IRQ handler.
*
* \param void : No parameter.
*
* \return : No return value.
*/
void SPI1_IRQHandler(void)
{
    /* If a transmission is on going, process the next transmit and receive bytes */
    if(((priv[0].reg->SR & SPI_SR_TXE_BIT_MASK) == SPI_SR_TXE_BIT_MASK) &&
       ((priv[0].reg->SR & SPI_SR_RXNE_BIT_MASK) == SPI_SR_RXNE_BIT_MASK))
    {
        if(priv[0].buffer_length > 0)
        {
            priv[0].read_buffer[priv[0].buffer_index] = priv[0].reg->DR;
            priv[0].buffer_length--;
            priv[0].buffer_index++;
            priv[0].reg->DR = priv[0].write_buffer[priv[0].buffer_index];
        }
        else
        {
            /* Disable TX buffer empty and RX buffer not empty interrupts */
            priv[0].reg->CR2 &= ~(SPI_CR2_TXEIE_BIT_MASK | SPI_CR2_RXNEIE_BIT_MASK);
            cs_high(priv[0].slave);
        }
    }
}

/** SPI2 IRQ handler.
*
* \param void : No parameter.
*
* \return : No return value.
*/
void SPI2_IRQHandler(void)
{
    /* If a transmission is on going, process the next transmit and receive bytes */
    if(((priv[1].reg->SR & SPI_SR_TXE_BIT_MASK) == SPI_SR_TXE_BIT_MASK) &&
       ((priv[1].reg->SR & SPI_SR_RXNE_BIT_MASK) == SPI_SR_RXNE_BIT_MASK))
    {
        if(priv[1].buffer_length > 0)
        {
            priv[1].read_buffer[priv[0].buffer_index] = priv[1].reg->DR;
            priv[1].buffer_length--;
            priv[1].buffer_index++;
            priv[1].reg->DR = priv[0].write_buffer[priv[1].buffer_index];
        }
        else
        {
            /* Disable TX buffer empty and RX buffer not empty interrupts */
            priv[1].reg->CR2 &= ~(SPI_CR2_TXEIE_BIT_MASK | SPI_CR2_RXNEIE_BIT_MASK);
            cs_high(priv[1].slave);
        }
    }
}
