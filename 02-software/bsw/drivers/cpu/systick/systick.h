/*
 * systick.h
 *
 * Created on: Aug 25, 2022
 * Author: Stephane Amans
 *
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

/* Include files:        */
#include <stdint.h>
#include <stdbool.h>

/* CPU Systick timer driver structure definition: */
struct t_systick_driver
{
    bool systick_on_processor_clock;
    uint32_t reload_value;
    struct
    {
        bool active;
        void (*callback)(void);
    }irq;
};

/** Disable the systick IRQ:
 *
 * \param void.
 *
 * \return: void.
 *
 */
void systick_disable_irq(void);

/** Enable the systick IRQ:
 *
 * \param void.
 *
 * \return: void.
 *
 */
void systick_enable_irq(void);

/** Get the systick parameters, current value and counter flag:
 *
 * \param driver: Return the current value to the pointer location.
 *
 * \return: bool, true if the counter reached 0.
 *
 */
bool systick_get_parameters(uint32_t *current_value);

/** Set the systick reload value:
 *
 * \param reload_value: Value to set in the systick reload register.
 *
 * \return: void.
 *
 */
void systick_set_reload_value(uint32_t reload_value);

/** Start the systick timer:
 *
 * \param void.
 *
 * \return: void.
 *
 */
void systick_start(void);

/** Stop the systick timer:
 *
 * \param void.
 *
 * \return: void.
 *
 */
void systick_stop(void);

/** Configure systick:
 *
 * \param config: Pointer to the systick driver structure.
 *
 * \return: void.
 *
 */
void systick_initialization(struct t_systick_driver *config);

#endif /* SYSTICK_H_ */

