/*
 * usart.c
 *
 * Created on: Apr 12, 2019
 * Author: Stephane Amans
 *
 * Changes:
 *  - Add DMA support Rx and Tx
 */

/* Include files        */
#include <string.h>

#include "usart.h"

#include "soc_configuration.h"

/* Defines */
#define USART_CR1_RE_BIT_MASK         0x4
#define USART_CR1_TE_BIT_MASK         0x8
#define USART_CR1_IDLEIE_BIT_MASK     0x10
#define USART_CR1_RXNEIE_BIT_MASK     0x20
#define USART_CR1_TCIE_BIT_MASK       0x40
#define USART_CR1_TXEIE_BIT_MASK      0x80
#define USART_CR1_PE_BIT_MASK         0x100
#define USART_CR1_PCE_BIT_MASK        0x400
#define USART_CR1_PS_BIT_MASK         0x200
#define USART_CR1_M_BIT_MASK          0x1000
#define USART_CR1_UE_BIT_MASK         0x2000

#define USART_CR2_0_5_STOP_BIT_MASK   0x1000
#define USART_CR2_2_STOP_BIT_MASK     0x2000
#define USART_CR2_STOP_BIT_MASK       0x3000

#define USART_CR3_DMAR_BIT_MASK       0x40
#define USART_CR3_DMAT_BIT_MASK       0x80

#define USART_SR_PE_BIT_MASK          0x1
#define USART_SR_FE_BIT_MASK          0x2
#define USART_SR_NE_BIT_MASK          0x4
#define USART_SR_ORE_BIT_MASK         0x8
#define USART_SR_IDLE_BIT_MASK        0x10
#define USART_SR_RXNE_BIT_MASK        0x20
#define USART_SR_TC_BIT_MASK          0x40
#define USART_SR_TXE_BIT_MASK         0x80

/* USART private structure definition :        */
struct t_usart_private
{
    struct t_usart_regs *reg;

    uint8_t *data_buffer_tx;
    uint32_t length_tx;
    uint32_t byte_counter_tx;

    uint8_t *data_buffer_rx;
    uint32_t length_rx;
    uint32_t byte_counter_rx;

    t_error_handling error;
    struct
    {
        void (*rx)(struct t_usart_driver *driver);
        void (*tx)(struct t_usart_driver *driver);
    }methods;

    struct t_dma_client client_rx;
    struct t_dma_client client_tx;
	struct t_dma_channel_driver *tx_dma;
    struct t_dma_channel_driver *rx_dma;
};

/* Static driver structure. */
static struct t_usart_private priv[USART_IP_NUMBER];

/** Compute divider from the bus frequency.
 *
 * \param bus_frequency: USART bus frequency value.
 *
 *
 * \return: t_error_handling code or ERROR_OK.
 *
 */
static uint16_t compute_baudrate_divider(uint32_t bus_frequency, uint32_t baudrate)
{
    uint32_t local_divider = baudrate * 16;
    uint16_t mantissa = bus_frequency / local_divider;
    uint8_t fraction = (uint8_t)(((bus_frequency % local_divider) * 16) / local_divider);

    if(fraction > 16)
    {
        mantissa++;
        fraction -= 16;
    }
    return ((mantissa << 4) | (fraction & 0x0F));
}

/** Interrupt function management.
 *
 * \param priv: Pointer to the private structure.
 *
 *
 * \return: void.
 *
 */
static void irq_management(struct t_usart_private *priv)
{
    /* Test the error bits */
    if((priv->reg->SR & USART_SR_PE_BIT_MASK) == USART_SR_PE_BIT_MASK)
    {
        priv->error = ERROR_USART_PARITY;
    }
    else if((priv->reg->SR & USART_SR_FE_BIT_MASK) == USART_SR_FE_BIT_MASK)
    {
        priv->error  = ERROR_USART_FRAMING;
    }
    else if((priv->reg->SR & USART_SR_NE_BIT_MASK) == USART_SR_NE_BIT_MASK)
    {
        priv->error  = ERROR_USART_NOISE;
    }
    else if((priv->reg->SR & USART_SR_ORE_BIT_MASK) == USART_SR_ORE_BIT_MASK)
    {
        priv->error  = ERROR_USART_OVERRUN;
    }
    else
    {
        /* If no error occured and RX interrupt arised */
        if((priv->reg->SR & USART_SR_RXNE_BIT_MASK) == USART_SR_RXNE_BIT_MASK)
        {
            /* Fill the buffer with the data received */
            priv->data_buffer_rx[priv->byte_counter_rx] = (uint8_t)priv->reg->DR;

            if(priv->length_rx == 0)
            {
                /* Deactivate interrupts if the data is received */
                priv->reg->CR1 &= ~(USART_CR1_IDLEIE_BIT_MASK | USART_CR1_RXNEIE_BIT_MASK | USART_CR1_PE_BIT_MASK | USART_CR1_RE_BIT_MASK);
            }
            else
            {
                /* Go to the next buffer address if the transmission is not ended */ 
                priv->length_rx--;
                priv->byte_counter_rx++;
            }
        }
        /* If no error occured and TX interrupt arised */
        else if(((priv->reg->SR & USART_SR_TXE_BIT_MASK) == USART_SR_TXE_BIT_MASK) &&
                 (priv->length_tx > 0))
        {
            /* Transfer the data to send into the shift register and go to the next buffer address. */
            priv->reg->DR = priv->data_buffer_tx[priv->byte_counter_tx];
            priv->byte_counter_tx++;
            priv->length_tx--;
        }
        else if((priv->reg->SR & USART_SR_TC_BIT_MASK) == USART_SR_TC_BIT_MASK)
        {
            /* If the transmission has ended, Deactivate the interrupts */
            priv->reg->CR1 &= ~(USART_CR1_IDLEIE_BIT_MASK | USART_CR1_TCIE_BIT_MASK | USART_CR1_TXEIE_BIT_MASK | USART_CR1_PE_BIT_MASK);
        }
    }
}

/** Function to receive bytes data with DMA.
 *
 * \param driver: Pointer to the USART driver.
 *
 * \return: void.
 *
 */
static void receive_dma(struct t_usart_driver *driver)
{
    /* Set the lasts client paramleters needed */
    driver->priv->client_rx.transfer_length = driver->priv->length_rx;
    driver->priv->client_rx.memory_address = (uintptr_t) driver->priv->data_buffer_rx;

    /* Set the the DMA channel and start the transfer */
    dma_set_transfer(driver->priv->rx_dma, &driver->priv->client_rx);
    driver->priv->reg->CR1 |= USART_CR1_RE_BIT_MASK;
    dma_start_transfer(driver->priv->rx_dma);
}  

/** Function to receive bytes data with interrupts.
 *
 * \param driver: Pointer to the USART driver.
 *
 * \return: void.
 *
 */
static void receive_irq(struct t_usart_driver *driver)
{
    /* Activate interruptions */
    driver->priv->reg->SR &= ~USART_SR_RXNE_BIT_MASK;
    driver->priv->reg->CR1 |= (USART_CR1_IDLEIE_BIT_MASK | USART_CR1_RXNEIE_BIT_MASK | USART_CR1_PE_BIT_MASK | USART_CR1_RE_BIT_MASK);
}

/** Polling function to read bytes data by polling.
 *
 * \param driver: Pointer to the USART driver.
 *
 * \return : void.
 *
 */
static void receive_poll(struct t_usart_driver *driver)
{
    /* Loop until the transfer is finished */
    while(driver->priv->byte_counter_rx < driver->priv->length_rx)
    {
        while((driver->priv->reg->SR & USART_SR_RXNE_BIT_MASK) == 0){}
        driver->priv->data_buffer_rx[driver->priv->byte_counter_rx] = (uint8_t)driver->priv->reg->DR;
        driver->priv->byte_counter_rx++;
        driver->priv->length_rx--;
    }
}

/** Function to transmit a bytes data with DMA.
 *
 * \param driver: Pointer to the USART driver.
 *
 * \return: void.
 *
 */
static void transmit_dma(struct t_usart_driver *driver)
{
    /* Set the lasts client paramleters needed */
    driver->priv->client_tx.transfer_length = driver->priv->length_tx;
    driver->priv->client_tx.memory_address = (uintptr_t) driver->priv->data_buffer_tx;

    /* Set the DMA channel and start the transfer */
    dma_set_transfer(driver->priv->tx_dma, &driver->priv->client_tx);
    dma_start_transfer(driver->priv->tx_dma);
}

/** Function to transmit bytes data with interrupts.
 *
 * \param driver: Pointer to the USART driver.
 *
 * \return: void.
 *
 */
static void transmit_irq(struct t_usart_driver *driver)
{
    driver->priv->reg->DR = driver->priv->data_buffer_tx[priv->byte_counter_tx];
    priv->length_tx--;
    priv->byte_counter_tx++;
    driver->priv->reg->CR1 |= (USART_CR1_IDLEIE_BIT_MASK | USART_CR1_TCIE_BIT_MASK | USART_CR1_TXEIE_BIT_MASK | USART_CR1_PE_BIT_MASK);
}


/** Blocking function to transmit bytes data.
 *
 * \param driver: Pointer to the USART driver.
 *
 * \return: void.
 *
 */
static void transmit_poll(struct t_usart_driver *driver)
{
    /* Loop until the transfer is finished */
    while(driver->priv->byte_counter_tx < driver->priv->length_tx)
    {
        driver->priv->reg->DR = driver->priv->data_buffer_tx[driver->priv->byte_counter_tx];
        while((driver->priv->reg->SR & USART_SR_TXE_BIT_MASK) != USART_SR_TXE_BIT_MASK){}
        driver->priv->byte_counter_tx++;
    }
    while((driver->priv->reg->SR & USART_SR_TC_BIT_MASK) != USART_SR_TC_BIT_MASK){}
    driver->priv->length_tx = 0;
}

void usart1_dma_ch4_irq_management(struct t_dma_status *dma_status)
{
    if(dma_status->transfer_complete == true)
    {
        priv[0].length_tx = 0;
    }
}

void usart1_dma_ch5_irq_management(struct t_dma_status *dma_status)
{
    if(dma_status->transfer_complete == true)
    {
        priv[0].length_rx = 0;
    }
}

t_error_handling usart_status(struct t_usart_driver *driver, bool read)
{
    t_error_handling error;

    /* Chack if the transmission ended */
    if((read == false) && (driver->priv->length_tx > 0))
    {
        error = ERROR_BUSY_TX;
    }

    /* Check if the reception ended */
    else if((read == true) && (driver->priv->length_rx > 0))  //(driver->priv->length_rx > 0))
    {
        error = ERROR_BUSY_RX;
    }
    else
    {
        /* If the transmission ended, check if any error occured */
        if((priv->reg->SR & USART_SR_PE_BIT_MASK) == USART_SR_PE_BIT_MASK)
        {
            priv->error = ERROR_USART_PARITY;
        }
        else if((priv->reg->SR & USART_SR_FE_BIT_MASK) == USART_SR_FE_BIT_MASK)
        {
            priv->error  = ERROR_USART_FRAMING;
        }
        else if((priv->reg->SR & USART_SR_NE_BIT_MASK) == USART_SR_NE_BIT_MASK)
        {
            priv->error  = ERROR_USART_NOISE;
        }
        else if((priv->reg->SR & USART_SR_ORE_BIT_MASK) == USART_SR_ORE_BIT_MASK)
        {
            priv->error  = ERROR_USART_OVERRUN;
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

t_error_handling usart_receive(struct t_usart_driver *driver, uint8_t *data_buffer, uint32_t data_length)
{
    /* Check usart status */
    driver->priv->error = usart_status(driver, true);

    /* If the peripheral is free */
    if(driver->priv->error == ERROR_OK)
    {
        /* Set the communication parameters and run the reception */
        driver->priv->length_rx = data_length;
        driver->priv->byte_counter_rx = 0;
        driver->priv->data_buffer_rx = data_buffer;
        driver->priv->reg->CR1 |= USART_CR1_RE_BIT_MASK;
        driver->priv->methods.rx(driver);
    }
    return driver->priv->error;
}

t_error_handling usart_transmit(struct t_usart_driver *driver, uint8_t *data_buffer, uint32_t data_length)
{
    /* Check usart status */
    driver->priv->error = usart_status(driver, false);

    /* If the peripheral is free */
    if(driver->priv->error == ERROR_OK)
    {
        /* Set the communication parameters and run the transmission */
        driver->priv->length_tx = data_length;
        driver->priv->byte_counter_tx = 0;
        driver->priv->data_buffer_tx = data_buffer;
        driver->priv->reg->CR1 |= USART_CR1_TE_BIT_MASK;
        driver->priv->methods.tx(driver);
    }
    return driver->priv->error;
}

void usart_initialization(struct t_usart_driver *config)
{
    uint32_t clock_frequency;

    /* Link private structure and clear it */
    config->priv = &priv[config->instance];
    memset(config->priv, 0, sizeof(struct t_usart_private));

    /* Link the registers structure */
    config->priv->reg = (struct t_usart_regs*)config->base_address;

    /* Enable periperal clock and set the clock value */
    enable_clock(config->peripheral);
    struct t_clock_driver *clock_driver = get_clock_driver();

    if(config->peripheral == USART1)
    {
        clock_frequency = clock_driver->APB2_clk_freq;
    }
    else
    {
        clock_frequency = clock_driver->APB1_clk_freq;
    }

    /* Module configuration */
    config->priv->reg->CR1 = USART_CR1_UE_BIT_MASK; /* Enables the module.        */
    config->priv->reg->BRR = compute_baudrate_divider(clock_frequency, config->baudrate);

    if(config->data_length == usart_data_9_bits)
    {
        config->priv->reg->CR1 |= USART_CR1_M_BIT_MASK;    /* Fix frame length.          */
    }

    if(config->parity == usart_parity_even)
    {
        config->priv->reg->CR1 |= USART_CR1_PCE_BIT_MASK;
    }

    else if(config->parity == usart_parity_odd)
    {
        config->priv->reg->CR1 |= (USART_CR1_PCE_BIT_MASK | USART_CR1_PS_BIT_MASK);
    }

    if(config->stop_bit == usart_one_stop_bit)
    {
        config->priv->reg->CR2 &= ~USART_CR2_STOP_BIT_MASK;
    }
    else if(config->stop_bit == usart_half_stop_bit)
    {
        config->priv->reg->CR2 |= USART_CR2_0_5_STOP_BIT_MASK;
    }
    else if(config->stop_bit == usart_two_stop_bit)
    {
        config->priv->reg->CR2 |= USART_CR2_2_STOP_BIT_MASK;
    }
    else if(config->stop_bit == usart_one_half_stop_bit)
    {
        config->priv->reg->CR2 |=  USART_CR2_STOP_BIT_MASK;
    }

    if(config->irq.active == true)
    {
        /* If IRQ mode is requested, enable IRQ and link TX/RX IRQ functions. */
        enable_nvic_irq(config->instance + NVIC_USART_OFFSET);
        set_nvic_priority(config->instance + NVIC_USART_OFFSET, config->irq.priority);
        config->priv->methods.tx = transmit_irq;
        config->priv->methods.rx = receive_irq;
    }
    else if(config->dma.active == true)
    {
        /* If DMA mode is requested, enable and parameter the DMA channel, link TX/RX DMA functions. */
        config->priv->reg->CR3 = (USART_CR3_DMAR_BIT_MASK | USART_CR3_DMAT_BIT_MASK); /* Enable DMA. */
        config->priv->tx_dma = config->dma.tx_channel;  /* Pointer to TX DMA driver. */
        config->priv->rx_dma = config->dma.rx_channel;  /* Pointer to RX DMA driver. */

        /* Set RX DMA client parameters */
        config->priv->client_rx.memory_increment = true;
        config->priv->client_rx.peripheral_increment = false;
        config->priv->client_rx.read_from_memory = false;
        config->priv->client_rx.mem_data_type = dma_8_bits;
        config->priv->client_rx.periph_data_type = dma_8_bits;
        config->priv->client_rx.peripheral_address = (uintptr_t)&config->priv->reg->DR;

        /* Set RX DMA client parameters */
        config->priv->client_tx.memory_increment = true;
        config->priv->client_tx.peripheral_increment = false;
        config->priv->client_tx.read_from_memory = true;
        config->priv->client_tx.mem_data_type = dma_8_bits;
        config->priv->client_tx.periph_data_type = dma_8_bits;
        config->priv->client_tx.peripheral_address = (uintptr_t)&config->priv->reg->DR;
        config->priv->methods.tx = transmit_dma;
        config->priv->methods.rx = receive_dma;
    }

    else
    {
        /* Default, the poll mode is requested, link TX/RX poll functions. */
        config->priv->methods.tx = transmit_poll;
        config->priv->methods.rx = receive_poll;
    }
}   

void usart_uninitialization(struct t_usart_driver *driver)
{
    /* Reset the module */
    reset_module(driver->peripheral);

    /* Disable periperal clock */
    disable_clock(driver->peripheral);

    /* Clear private structure */
    memset(driver->priv, 0, sizeof(struct t_usart_private));
}   

/** USART1 IRQ handler.
*
* \param void : No parameter.
*
* \return : No return value.
*/
void USART1_IRQHandler(void)
{
    irq_management(&priv[0]);
    clear_pending_nvic_irq(IRQ_USART_1); /* Clear any USART 1 NVIC pending interrupt.   */
}

/** USART2 IRQ handler.
*
* \param void : No parameter.
*
* \return : No return value.
*/
void USART2_IRQHandler(void)
{
    irq_management(&priv[1]);
    clear_pending_nvic_irq(IRQ_USART_2); /* Clear any USART 1 NVIC pending interrupt.   */
}
