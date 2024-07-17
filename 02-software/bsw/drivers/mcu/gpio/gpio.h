/*
 * gpio.h
 *
 * Created on: Apr 12, 2019
 * Author: Stephane Amans
 *
 */
#ifndef GPIO_H_
#define GPIO_H_

/* Include files:        */
#include <stdbool.h>
#include <stdint.h>

#include "driver_common.h"
#include "clock.h"
#include "nvic.h"
#include "exti.h"
#include "regbase_gpio.h"

#include "soc_configuration.h"

/* MODE, GPIO configuration mode and configuration:  */
enum t_gpio_type
{
    input_analog,
    input_floating,
    input_pullup,
    input_pulldown,
    output_pushpull_2MHz,
    output_pushpull_10MHz,
    output_pushpull_50MHz,
    output_opendrain_2MHz,
    output_opendrain_10MHz,
    output_opendrain_50MHz,
    output_altpushpull_2MHz,
    output_altpushpull_10MHz,
    output_altpushpull_50MHz,
    output_altopendrain_2MHz,
    output_altopendrain_10MHz,
    output_altopendrain_50MHz
};

/* GPIO initialization structure definition :        */
struct t_gpio_driver
{
    uintptr_t base_address_gpio;
    const struct t_exti_config *exti_config;
    const struct t_afio_config *afio_config;
    uint8_t pin;
    enum t_gpio_type type;
    enum t_peripheral peripheral;
    uint16_t instance;
    struct
    {
        bool active;
        bool rising;
        uint8_t priority;
        void(*callback)(uint32_t);
    }irq;
    struct t_gpio_private *priv;
};

/* Functions prototypes:                       */

/** Disable GPIO interruption:
 *
 * \param exti_number: number of the interruption line (EXTI) to disable.
 *
 * \return: void.
 *
 */
void gpio_disable_irq(uint8_t exti_number);

/** Read a GPIO.
*
* \param driver: Pointer to the driver structure.
*
* \return: return the pin value.
*
*/
bool gpio_read(struct t_gpio_driver *driver);

/** Toggle a GPIO.
*
* \param driver: Pointer to the driver structure.
*
* \return: void.
*
*/
void gpio_toggle(struct t_gpio_driver *driver);

/** Set a GPIO.
*
* \param driver: Pointer to the driver structure.
* \param state: pin state (high level if true)
*
* \return: void.
*
*/
void gpio_write(struct t_gpio_driver *driver, bool state);

/** Configure GPIO (port, pin, in/out, mode, interruption):
 *
 * \param driver: Pointer to the driver structure.
 *
 * \return: void.
 *
 */
void gpio_initialization(struct t_gpio_driver *driver);

#endif /* GPIO_H_ */
