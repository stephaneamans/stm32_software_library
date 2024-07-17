/*
 * dma.c
 *
 * Created on: Jun 8, 2020
 * Author: Stephane Amans
 *
 *    Change log:
 *
 *    29/07/2022: Refactor driver
 *
 */

/* Include files.        */
#include "clock.h"
#include "dma.h"
#include "nvic.h"
#include "regbase_dma.h"

#include "soc_configuration.h"

/* Bits masks */
#define DMA_CCR_EN_BIT_MASK        0x1U
#define DMA_CCR_TCIE_BIT_MASK      0x2U
#define DMA_CCR_HTIE_BIT_MASK      0x4U
#define DMA_CCR_TEIE_BIT_MASK      0x8U
#define DMA_CCR_DIR_BIT_MASK       0x10U
#define DMA_CCR_PINC_BIT_MASK      0x40U
#define DMA_CCR_MINC_BIT_MASK      0x80U
#define DMA_CCR_PSIZE_16BITS_MASK  0x100U
#define DMA_CCR_PSIZE_32BITS_MASK  0x200U
#define DMA_CCR_PSIZE_BIT_MASK     0x300U
#define DMA_CCR_MSIZE_16BITS_MASK  0x400U
#define DMA_CCR_MSIZE_32BITS_MASK  0x800U
#define DMA_CCR_MSIZE_BIT_MASK     0xC00U
#define DMA_CCR_PL_MEDIUM_BIT_MASK 0x1000U
#define DMA_CCR_PL_HIGH_BIT_MASK   0x2000U
#define DMA_CCR_PL_V_HIGH_BIT_MASK 0x3000U
#define DMA_CCR_PL_BIT_MASK        0x3000U
#define DMA_CCR_MEM2MEM_BIT_MASK   0x4000U

#define DMA_ISR_IFCR_CH1_MASK      0xFU
#define DMA_ISR_IFCR_CH2_MASK      0xF0U
#define DMA_ISR_IFCR_CH3_MASK      0xF00U
#define DMA_ISR_IFCR_CH4_MASK      0xF000U
#define DMA_ISR_IFCR_CH5_MASK      0xF0000U
#define DMA_ISR_IFCR_CH6_MASK      0xF00000U
#define DMA_ISR_IFCR_CH7_MASK      0xF000000U
#define DMA_ISR_CH1_TCIF_BITMASK   0x2U
#define DMA_ISR_CH1_HTIF_BITMASK   0x4U
#define DMA_ISR_CH1_TEIF_BITMASK   0x8U
#define DMA_ISR_CH2_TCIF_BITMASK   0x20U
#define DMA_ISR_CH2_HTIF_BITMASK   0x40U
#define DMA_ISR_CH2_TEIF_BITMASK   0x80U
#define DMA_ISR_CH3_TCIF_BITMASK   0x200U
#define DMA_ISR_CH3_HTIF_BITMASK   0x400U
#define DMA_ISR_CH3_TEIF_BITMASK   0x800U
#define DMA_ISR_CH4_TCIF_BITMASK   0x2000U
#define DMA_ISR_CH4_HTIF_BITMASK   0x4000U
#define DMA_ISR_CH4_TEIF_BITMASK   0x8000U
#define DMA_ISR_CH5_TCIF_BITMASK   0x20000U
#define DMA_ISR_CH5_HTIF_BITMASK   0x40000U
#define DMA_ISR_CH5_TEIF_BITMASK   0x80000U
#define DMA_ISR_CH6_TCIF_BITMASK   0x200000U
#define DMA_ISR_CH6_HTIF_BITMASK   0x400000U
#define DMA_ISR_CH6_TEIF_BITMASK   0x800000U
#define DMA_ISR_CH7_TCIF_BITMASK   0x2000000U
#define DMA_ISR_CH7_HTIF_BITMASK   0x4000000U
#define DMA_ISR_CH7_TEIF_BITMASK   0x8000000U

/* DMA status bitfield structure definition :  */
struct t_dma_isr_bit
{
    uint8_t gif : 1;
    uint8_t tcif : 1;
    uint8_t htif : 1;
    uint8_t teif : 1;
};

/* DMA private structures definition :        */
struct t_dma_private
{
    struct t_dma_regs *reg;
};

struct t_dma_channel_private
{
    struct t_dma_channel_regs *channel_reg;
    uint8_t channel_number;
    struct t_dma_driver *dma;
};

/* Static DMA private structure instances. */
static struct t_dma_private priv[DMA_IP_NUMBER];
static struct t_dma_channel_private priv_channel[DMA_CHANNEL_NUMBER];

/* Pointer callback function table prototype    */
void (*dma_callback[DMA_CHANNEL_NUMBER])(struct t_dma_status *dma_status);

static void dma_irq_management(struct t_dma_channel_private *channel_priv, struct t_dma_status *dma_status)
{
    /* Store and left shift to the 4 LSB the concerned bits of the ISR register */
    uint32_t dma_isr = priv[channel_priv->dma->instance].reg->ISR;
    dma_isr = dma_isr >> (channel_priv->channel_number * 4);

    /* Fill the ISR bitfield structure */
    struct t_dma_isr_bit *isr_bit = (struct t_dma_isr_bit*)&dma_isr;

    /* Fill the DMA status structure fields */
    dma_status->transfer_complete = isr_bit->tcif;
    dma_status->half_transfer_complete = isr_bit->htif;
    dma_status->transfer_error = isr_bit->teif;

    /* Clear the dedicated ISR flag */
    priv[channel_priv->dma->instance].reg->IFCR = DMA_ISR_IFCR_CH1_MASK << (channel_priv->channel_number * 4);
}

bool dma_get_half_transfer_complete(struct t_dma_channel_driver *driver)
{
    bool bit_value;

    /* Store and left shift to the 4 LSB the concerned bits of the ISR register */
    uint32_t dma_isr = driver->priv->dma->priv->reg->ISR;
    dma_isr = dma_isr >> (driver->channel_number * 4);

    /* Fill the ISR bitfield structure */
    struct t_dma_isr_bit *isr_bit = (struct t_dma_isr_bit*)&dma_isr;
    bit_value = isr_bit->htif;

    /* Clear the dedicated ISR flag */
    driver->priv->dma->priv->reg->IFCR = DMA_ISR_CH1_HTIF_BITMASK << (driver->channel_number * 4);

    return bit_value;
}

bool dma_get_transfer_complete(struct t_dma_channel_driver *driver)
{
    bool bit_value;

    /* Store and left shift to the 4 LSB the concerned bits of the ISR register */
    uint32_t dma_isr = driver->priv->dma->priv->reg->ISR;
    dma_isr = dma_isr >> (driver->channel_number * 4);

    /* Fill the ISR bitfield structure */
    struct t_dma_isr_bit *isr_bit = (struct t_dma_isr_bit*)&dma_isr;
    bit_value = isr_bit->tcif;

    /* Clear the dedicated ISR flag */
    driver->priv->dma->priv->reg->IFCR = DMA_ISR_CH1_TCIF_BITMASK << (driver->channel_number * 4);

    return bit_value;
}

bool dma_get_transfer_error(struct t_dma_channel_driver *driver)
{
    bool bit_value;

    /* Store and left shift to the 4 LSB the concerned bits of the ISR register */
    uint32_t dma_isr = driver->priv->dma->priv->reg->ISR;
    dma_isr = dma_isr >> (driver->channel_number * 4);

    /* Fill the ISR bitfield structure */
    struct t_dma_isr_bit *isr_bit = (struct t_dma_isr_bit*)&dma_isr;
    bit_value = isr_bit->teif;

    /* Clear the dedicated ISR flag */
    driver->priv->dma->priv->reg->IFCR = DMA_ISR_CH1_TEIF_BITMASK << (driver->channel_number * 4);

    return bit_value;
}

void dma_set_transfer(struct t_dma_channel_driver *driver, struct t_dma_client *client)
{
    dma_stop_transfer(driver);

    /* Clear the configuration register before setting the new parameters. */
    driver->priv->channel_reg->CCR &= ~(DMA_CCR_MSIZE_BIT_MASK |
                                        DMA_CCR_PSIZE_BIT_MASK |
                                        DMA_CCR_MINC_BIT_MASK |
                                        DMA_CCR_PINC_BIT_MASK |
                                        DMA_CCR_DIR_BIT_MASK);

    /* Set the new configuration parameters. */
    if(client->memory_increment == true)
    {
        driver->priv->channel_reg->CCR |= DMA_CCR_MINC_BIT_MASK;
    }

    if(client->peripheral_increment == true)
    {
        driver->priv->channel_reg->CCR |= DMA_CCR_PINC_BIT_MASK;
    }

    if(client->read_from_memory == true)
    {
        driver->priv->channel_reg->CCR |= DMA_CCR_DIR_BIT_MASK;
    }

    if(client->periph_data_type == dma_16_bits)
    {
        driver->priv->channel_reg->CCR |= DMA_CCR_PSIZE_16BITS_MASK;
    }
    else if(client->periph_data_type == dma_32_bits)
    {
        driver->priv->channel_reg->CCR |= DMA_CCR_PSIZE_32BITS_MASK;
    }

    if(client->mem_data_type == dma_16_bits)
    {
        driver->priv->channel_reg->CCR |= DMA_CCR_MSIZE_16BITS_MASK;
    }
    else if(client->mem_data_type == dma_32_bits)
    {
        driver->priv->channel_reg->CCR |= DMA_CCR_MSIZE_32BITS_MASK;
    }

    /* Set the new memory address. */
    driver->priv->channel_reg->CMAR = (uint32_t)client->memory_address;

    /* Set the new peripheral address. */
    driver->priv->channel_reg->CPAR = (uint32_t)client->peripheral_address;

    /* Set the new length. */
    driver->priv->channel_reg->CNDTR = client->transfer_length;
}

void dma_start_transfer(struct t_dma_channel_driver *driver)
{
    driver->priv->channel_reg->CCR |= DMA_CCR_EN_BIT_MASK;      /* Enable the DMA channel.  */
}

void dma_stop_transfer(struct t_dma_channel_driver *driver)
{
    driver->priv->channel_reg->CCR &= ~DMA_CCR_EN_BIT_MASK;     /* Disable the DMA channel. */
}

void dma_transfer_status(struct t_dma_channel_driver *driver, struct t_dma_status *dma_status)
{
    /* Store and left shift to the 4 LSB the concerned bits of the ISR register */
    uint32_t dma_isr = driver->priv->dma->priv->reg->ISR;
    dma_isr = dma_isr >> (driver->channel_number * 4);

    /* Fill the ISR bitfield structure */
    struct t_dma_isr_bit *isr_bit = (struct t_dma_isr_bit*)&dma_isr;

    /* Fill the DMA status structure fields */
    dma_status->transfer_complete = isr_bit->tcif;
    dma_status->half_transfer_complete = isr_bit->htif;
    dma_status->transfer_error = isr_bit->teif;

    /* Clear the dedicated ISR flag */
    driver->priv->dma->priv->reg->IFCR = DMA_ISR_IFCR_CH1_MASK << (driver->channel_number * 4);
}

void dma_initialization(struct t_dma_driver *config, struct t_dma_channel_driver *channel_config)
{
    /* Affects the private structure instance to the private driver pointer */
    config->priv = &priv[config->instance];
    channel_config->priv = &priv_channel[channel_config->channel_number];

    /* Initializes the private structure instances */
    config->priv->reg = (struct t_dma_regs*)config->base_address_dma;
    channel_config->priv->channel_reg = (struct t_dma_channel_regs*)channel_config->base_address_dma_channel;
    channel_config->priv->dma = config;
    channel_config->priv->channel_number = channel_config->channel_number;

    enable_clock(config->peripheral);

    /* Configure the DMA channel */
    channel_config->priv->channel_reg->CCR = 0x00; /* Clear the DMA channel configurarion register.   */

    if(channel_config->mem2mem == true)
    {
        channel_config->priv->channel_reg->CCR |= DMA_CCR_MEM2MEM_BIT_MASK;
    }

    if(channel_config->dma_priority_level == 1)
    {
        channel_config->priv->channel_reg->CCR |= DMA_CCR_PL_MEDIUM_BIT_MASK;
    }
    else if(channel_config->dma_priority_level == 2)
    {
        channel_config->priv->channel_reg->CCR |= DMA_CCR_PL_HIGH_BIT_MASK;
    }
    else if(channel_config->dma_priority_level == 3)
    {
        channel_config->priv->channel_reg->CCR |= DMA_CCR_PL_V_HIGH_BIT_MASK;
    }

    if(channel_config->irq.transfer_complete == true)
    {
        channel_config->priv->channel_reg->CCR |= DMA_CCR_TCIE_BIT_MASK;
    }
     
    if(channel_config->irq.half_transfer_complete == true)
    {
        channel_config->priv->channel_reg->CCR |= DMA_CCR_HTIE_BIT_MASK;
    }
    
    if(channel_config->irq.transfer_error == true)
    {
        channel_config->priv->channel_reg->CCR |= DMA_CCR_TEIE_BIT_MASK;
    }

    if((channel_config->irq.transfer_complete == true) ||
       (channel_config->irq.half_transfer_complete == true) ||
       (channel_config->irq.transfer_error == true))
    {
        enable_nvic_irq(channel_config->channel_number + IRQ_DMA1_CHANNEL1);
        set_nvic_priority(channel_config->channel_number + IRQ_DMA1_CHANNEL1, channel_config->irq.priority);
        dma_callback[channel_config->channel_number] = channel_config->irq.callback;
    }
}

/** DMA1 channel 1 IRQ handler.
*
* \param void: No parameter.
*
* \return: No return value.
*/
extern void DMA1_Channel1_IRQHandler(void)
{
    if(dma_callback[0] != 0)
    {
        struct t_dma_status dma_status;
        dma_irq_management(&priv_channel[0], &dma_status);
        dma_callback[0](&dma_status);          /* Call the DMA0 subroutine.              */
    }
    clear_pending_nvic_irq(IRQ_DMA1_CHANNEL1); /* Clear any DMA0 NVIC pending interrupt. */
}

/** DMA1 channel 2 IRQ handler.
*
* \param void: No parameter.
*
* \return: No return value.
*/
extern void DMA1_Channel2_IRQHandler(void)
{
    if(dma_callback[1] != 0)
    {
        struct t_dma_status dma_status;
        dma_irq_management(&priv_channel[1], &dma_status);
        dma_callback[1](&dma_status);          /* Call the DMA0 subroutine.              */
    }
    clear_pending_nvic_irq(IRQ_DMA1_CHANNEL2); /* Clear any DMA0 NVIC pending interrupt. */
}

/** DMA1 channel 3 IRQ handler.
*
* \param void: No parameter.
*
* \return: No return value.
*/
extern void DMA1_Channel3_IRQHandler(void)
{
    if(dma_callback[2] != 0)
    {
        struct t_dma_status dma_status;
        dma_irq_management(&priv_channel[2], &dma_status);
        dma_callback[2](&dma_status);          /* Call the DMA0 subroutine.              */
    }
    clear_pending_nvic_irq(IRQ_DMA1_CHANNEL3); /* Clear any DMA0 NVIC pending interrupt. */
}

/** DMA1 channel 4 IRQ handler.
*
* \param void: No parameter.
*
* \return: No return value.
*/
extern void DMA1_Channel4_IRQHandler(void)
{
    if(dma_callback[3] != 0)
    {
        struct t_dma_status dma_status;
        dma_irq_management(&priv_channel[3], &dma_status);
        dma_callback[3](&dma_status);          /* Call the DMA0 subroutine.              */
    }
    clear_pending_nvic_irq(IRQ_DMA1_CHANNEL4); /* Clear any DMA0 NVIC pending interrupt. */
}

/** DMA1 channel 5 IRQ handler.
*
* \param void: No parameter.
*
* \return: No return value.
*/
extern void DMA1_Channel5_IRQHandler(void)
{
    if(dma_callback[4] != 0)
    {
        struct t_dma_status dma_status;
        dma_irq_management(&priv_channel[4], &dma_status);
        dma_callback[4](&dma_status);          /* Call the DMA0 subroutine.              */
    }
    clear_pending_nvic_irq(IRQ_DMA1_CHANNEL5); /* Clear any DMA0 NVIC pending interrupt. */
}

/** DMA1 channel 6 IRQ handler.
*
* \param void: No parameter.
*
* \return: No return value.
*/
extern void DMA1_Channel6_IRQHandler(void)
{
    if(dma_callback[5] != 0)
    {
        struct t_dma_status dma_status;
        dma_irq_management(&priv_channel[5], &dma_status);
        dma_callback[5](&dma_status);          /* Call the DMA0 subroutine.              */
    }
    clear_pending_nvic_irq(IRQ_DMA1_CHANNEL6); /* Clear any DMA0 NVIC pending interrupt. */
}

/** DMA1 channel 7 IRQ handler.
*
* \param void: No parameter.
*
* \return: No return value.
*/
extern void DMA1_Channel7_IRQHandler(void)
{
    if(dma_callback[6] != 0)
    {
        struct t_dma_status dma_status;
        dma_irq_management(&priv_channel[6] , &dma_status);
        dma_callback[6](&dma_status);          /* Call the DMA0 subroutine.              */
    }
    clear_pending_nvic_irq(IRQ_DMA1_CHANNEL7); /* Clear any DMA0 NVIC pending interrupt. */
}
