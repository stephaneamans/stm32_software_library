/*
 * gpio.c
 *
 * Created on: Apr 12, 2019
 * Author: Stephane Amans
 *
 *    Change log:
 *
 *    29/01/2022: Refactor driver
 *
 */

/* Include files.        */
#include <string.h>

#include "gpio.h"
#include "afio.h"
#include "exti.h"

#define INPUT_ANALOG                 0x00
#define INPUT_FLOATING               0x01
#define INPUT_PUPD                   0x02

#define OUTPUT_PUSH_PULL             0x00
#define OUTPUT_OPEN_DRAIN            0x01
#define OUTPUT_ALTERNATE_PUSH_PULL   0x02
#define OUTPUT_ALTERNATE_OPEN_DRAIN  0x03

#define INPUT           0x00
#define OUTPUT_10_MHZ   0x01
#define OUTPUT_2_MHZ    0x02
#define OUTPUT_50_MHZ   0x03

#define GPIO_CRx_MODE_MASK  0x3
#define GPIO_CRx_CNF_MASK   0xC

/* GPIO private structure definition :        */
struct t_gpio_private_global
{
    struct t_exti_regs *exti_reg;
};

struct t_gpio_private
{
    struct t_gpio_regs *gpio_reg;
    uint16_t gpio_pin_mask;
};

/* Static GPIO private structure instances. */
static struct t_gpio_private_global priv_g;
static struct t_gpio_private priv[GPIO_PIN_NUMBER];

/* Pointer callback function table prototype	*/
void (*gpio_callback[EXTI_CHANNEL_NUMBER])(uint32_t pin_numb);

static void set_gpio_irq(struct t_gpio_driver *config)
{
    struct t_afio_regs *afio_reg = (struct t_afio_regs*)config->afio_config->base_address_afio;

    /* Clear the 4 bits corresponding to the port selection. */
    afio_reg->EXTICR[config->pin / 4] &= ~(0xF << ((config->pin % 4) * 4));

    /* Set the localPort variable function of the port number. */
    switch(config->peripheral)
    {
        case PORT_B:
            afio_reg->EXTICR[config->pin / 4] |= 1 << ((config->pin % 4) * 4);
            break;

        case PORT_C:
            afio_reg->EXTICR[config->pin / 4] |= 2 << ((config->pin % 4) * 4);
            break;

        case PORT_D:
            afio_reg->EXTICR[config->pin / 4] |= 3 << ((config->pin % 4) * 4);
            break;

        case PORT_E:
            afio_reg->EXTICR[config->pin / 4] |= 4 << ((config->pin % 4) * 4);
            break;

        case PORT_F:
            afio_reg->EXTICR[config->pin / 4] |= 5 << ((config->pin % 4) * 4);
            break;

        case PORT_G:
            afio_reg->EXTICR[config->pin / 4] |= 6 << ((config->pin % 4) * 4);
            break;

        default:
            break;
    }    

    priv_g.exti_reg->IMR |= config->priv->gpio_pin_mask;     /* Unmask interrupt line. */
    priv_g.exti_reg->EMR |= config->priv->gpio_pin_mask;     /* Unmask event line.     */

    /* Associate the callbacks references. */
    if(config->pin <= 4)
    {
        enable_nvic_irq(config->pin + NVIC_GPIO_OFFSET);
        set_nvic_priority(config->pin + NVIC_GPIO_OFFSET, config->irq.priority);
        gpio_callback[config->pin] = config->irq.callback;
    }

    /* Set all parameters to configure interrupts on rising edge.*/
    if(config->irq.rising == true)
    {
        priv_g.exti_reg->RTSR |= config->priv->gpio_pin_mask;   /* Interrupt / event on rising edge.          */
        priv_g.exti_reg->FTSR &= ~config->priv->gpio_pin_mask;  /* Disable interrupt / event on falling edge. */
    }

    /* Set all parameters to configure interrupts on falling edge. */
    else
    {
        priv_g.exti_reg->RTSR &= ~config->priv->gpio_pin_mask;  /* Disable interrupt / event on rising edge. */
        priv_g.exti_reg->FTSR |= config->priv->gpio_pin_mask;   /* Interrupt / event on falling edge.        */
    }
}

void gpio_disable_irq(uint8_t exti_number)
{
    uint32_t local_mask = 1 << exti_number;
    priv_g.exti_reg->IMR &= ~local_mask;  /* Mask interrupt line.                       */
    priv_g.exti_reg->EMR &= ~local_mask;  /* Mask event line.                           */
    priv_g.exti_reg->FTSR &= ~local_mask; /* Disable interrupt / event on falling edge. */
    priv_g.exti_reg->RTSR &= ~local_mask; /* Disable interrupt / event on rising edge.  */
    disable_nvic_irq(exti_number + 6);
    gpio_callback[exti_number] = 0;
}

bool gpio_read(struct t_gpio_driver *driver)
{
    bool ret_val = false;
    if((driver->priv->gpio_reg->IDR & driver->priv->gpio_pin_mask) == driver->priv->gpio_pin_mask)
    {
        ret_val = true;
    }
    return (ret_val);
}

void gpio_toggle(struct t_gpio_driver *driver)
{
    if((driver->priv->gpio_reg->ODR & driver->priv->gpio_pin_mask) == driver->priv->gpio_pin_mask)      /* If the pin is high level */
    {
        driver->priv->gpio_reg->BRR |= driver->priv->gpio_pin_mask;                                     /* then clear the pin.      */
    }
    else if((driver->priv->gpio_reg->ODR & driver->priv->gpio_pin_mask) != driver->priv->gpio_pin_mask) /* If the pin is low level  */
    {
        driver->priv->gpio_reg->BSRR |= driver->priv->gpio_pin_mask;                                    /* then set the pin.        */
    }
}

void gpio_write(struct t_gpio_driver *driver, bool state)
{
    if(state == true)
    {
        driver->priv->gpio_reg->BSRR |= driver->priv->gpio_pin_mask;
    }
    else
    {
        driver->priv->gpio_reg->BRR |= driver->priv->gpio_pin_mask;
    }
}

void gpio_initialization(struct t_gpio_driver *config)
{
    uint32_t local_mask = 0; /* Local variable for masks operations. */

    /* Affects the private structure instance to the private driver pointer */
    config->priv = &priv[config->instance];

    /* Initializes the private structure instances */
    config->priv->gpio_reg = (struct t_gpio_regs*)config->base_address_gpio;
    priv_g.exti_reg = (struct t_exti_regs*)config->exti_config->base_address_exti;
    config->priv->gpio_pin_mask = 1 << config->pin;

    enable_clock(config->peripheral);

    /* Set GPIO mask if configured as input */
    if (config->type == input_floating)
    {
        local_mask = (INPUT_FLOATING << 2) | INPUT;
    }

    else if (config->type == input_pullup)
    {
        local_mask = (INPUT_PUPD << 2) | INPUT;
        config->priv->gpio_reg->ODR |= config->priv->gpio_pin_mask;
    }

    else if (config->type == input_pulldown)
    {
        local_mask = (INPUT_PUPD << 2) | INPUT;
        config->priv->gpio_reg->ODR &= ~config->priv->gpio_pin_mask;
    }

    /* Set GPIO mask if configured as output */
    if ((config->type == output_pushpull_2MHz) ||
        (config->type == output_pushpull_10MHz) ||
        (config->type == output_pushpull_50MHz))
    {
        local_mask = OUTPUT_PUSH_PULL << 2;
    }

    else if ((config->type == output_opendrain_2MHz) ||
             (config->type == output_opendrain_10MHz) ||
             (config->type == output_opendrain_50MHz))
    {
        local_mask = OUTPUT_OPEN_DRAIN << 2;
    }

    else if ((config->type == output_altpushpull_2MHz) ||
             (config->type == output_altpushpull_10MHz) ||
             (config->type == output_altpushpull_50MHz))
    {
        local_mask = OUTPUT_ALTERNATE_PUSH_PULL << 2;
    }

    else if ((config->type == output_altopendrain_2MHz) ||
             (config->type == output_altopendrain_10MHz) ||
             (config->type == output_altopendrain_50MHz))
    {
        local_mask = OUTPUT_ALTERNATE_OPEN_DRAIN << 2;
    }

    if ((config->type == output_pushpull_2MHz) ||
        (config->type == output_opendrain_2MHz) ||
        (config->type == output_altpushpull_2MHz) ||
        (config->type == output_altopendrain_2MHz))
    {
        local_mask |= OUTPUT_2_MHZ;
    }

    else if ((config->type == output_pushpull_10MHz) ||
             (config->type == output_opendrain_10MHz) ||
             (config->type == output_altpushpull_10MHz) ||
             (config->type == output_altopendrain_10MHz))
    {
        local_mask |= OUTPUT_10_MHZ;
    }

    else if ((config->type == output_pushpull_50MHz) ||
             (config->type == output_opendrain_50MHz) ||
             (config->type == output_altpushpull_50MHz) ||
             (config->type == output_altopendrain_50MHz))
    {
        local_mask |= OUTPUT_50_MHZ;
    }

    /* Write mask in the low register for GPIO 0 to 7.    */
    if(config->pin <= 7)
    {
        /* Clear the dedicated field in the register */
        config->priv->gpio_reg->CRL &= ~((GPIO_CRx_MODE_MASK | GPIO_CRx_CNF_MASK) << (config->pin * 4));
        /* Set the expected GPIO configuration in the register */
        config->priv->gpio_reg->CRL |= local_mask << (config->pin * 4);
    }

    /* Write mask in the high register for GPIO 8 to 15. */
    else if(config->pin >= 8 && config->pin <= 15)
    {
        /* Clear the dedicated field in the register */
        config->priv->gpio_reg->CRH &= ~((GPIO_CRx_MODE_MASK | GPIO_CRx_CNF_MASK) << ((config->pin -8 ) * 4));
        /* Set the expected GPIO configuration in the register */
        config->priv->gpio_reg->CRH |= local_mask<<((config->pin -8 ) * 4);
    }

    /* Enables GPIO interrupts if they are allowed */
    if((config->irq.active == true) &&
       ((config->type == input_floating) ||
        (config->type == input_pullup) ||
        (config->type == input_pulldown)))

    {
        set_gpio_irq(config);
    }
}

/** GPIO0 IRQ handler.
*
* \param void: No parameter.
*
* \return: No return value.
*/
extern void EXTI0_IRQHandler(void)
{
    if(gpio_callback[0] != 0)
    {
        gpio_callback[0](priv_g.exti_reg->PR); /* Call the GPIO0 subroutine.                  */
    }
    clear_pending_nvic_irq(IRQ_EXTI_0);        /* Clear any GPIO0 NVIC pending interrupt.     */
    priv_g.exti_reg->PR = 0xFFFFFFFF;          /* Clear all GPIO0 pending interrupt flags.    */
}

/** GPIO1 IRQ handler.
*
* \param void: No parameter.
*
* \return: No return value.
*/
extern void EXTI1_IRQHandler(void)
{
    if(gpio_callback[1] != 0)
    {
        gpio_callback[1](priv_g.exti_reg->PR); /* Call the GPIO1 subroutine.                  */
    }
    clear_pending_nvic_irq(IRQ_EXTI_1);        /* Clear any GPIO1 NVIC pending interrupt.     */
    priv_g.exti_reg->PR = 0xFFFFFFFF;          /* Clear all GPIO1 pending interrupt flags.    */
}

/** GPIO2 IRQ handler.
*
* \param void: No parameter.
*
* \return : No return value.
*/
extern void EXTI2_IRQHandler(void)
{
    if(gpio_callback[2] != 0)
    {
        gpio_callback[2](priv_g.exti_reg->PR); /* Call the GPIO2 subroutine.                  */
    }
    clear_pending_nvic_irq(IRQ_EXTI_2);        /* Clear any GPIO2 NVIC pending interrupt.     */
    priv_g.exti_reg->PR = 0xFFFFFFFF;          /* Clear all GPIO2 pending interrupt flags.    */
}

/** GPIO3 IRQ handler.
*
* \param void : No parameter.
*
* \return : No return value.
*/
extern void EXTI3_IRQHandler(void)
{
    if(gpio_callback[3] != 0)
    {
        gpio_callback[3](priv_g.exti_reg->PR); /* Call the GPIO3 subroutine.                  */
    }
    clear_pending_nvic_irq(IRQ_EXTI_3);        /* Clear any GPIO3 NVIC pending interrupt.     */
    priv_g.exti_reg->PR = 0xFFFFFFFF;          /* Clear all GPIO3 pending interrupt flags.    */
}

/** GPIO4 IRQ handler.
*
* \param void : No parameter.
*
* \return : No return value.
*/
extern void EXTI4_IRQHandler(void)
{
    if(gpio_callback[4] != 0)
    {
        gpio_callback[4](priv_g.exti_reg->PR); /* Call the GPIO4 subroutine.                  */
    }
    clear_pending_nvic_irq(IRQ_EXTI_4);        /* Clear any GPIO4 NVIC pending interrupt.     */
    priv_g.exti_reg->PR = 0xFFFFFFFF;          /* Clear all GPIO4 pending interrupt flags.    */
}
