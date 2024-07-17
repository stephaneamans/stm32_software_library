/*
 * systick.c
 *
 * Created on: Aug 25, 2022
 * Author: Stephane Amans
 *
 */

/* Include files        */
#include "systick.h"

/* Peripheral SYSTICK base pointer address */
#define SYSTICK   ((volatile struct t_sysclock_regs *)(0xE000E010u))

/* Defines */
#define SYSTICK_CSR_ENABLE_BIT_MASK          0x1
#define SYSTICK_CSR_TICKINT_BIT_MASK         0x2
#define SYSTICK_CSR_CLKSOURCE_BIT_MASK       0x4
#define SYSTICK_CSR_COUNTFLAG_BIT_MASK       0x10000

/* SYSTICK registers structure        */
struct t_sysclock_regs
{
    volatile uint32_t CSR;
    volatile uint32_t RVR;
    volatile uint32_t CVR;
    volatile uint32_t CALIB;
};

/* Pointer callback function prototype    */
void (*systick_callback)(void);

void systick_disable_irq(void)
{
    SYSTICK->CSR &= ~SYSTICK_CSR_TICKINT_BIT_MASK;
}

void systick_enable_irq(void)
{
    SYSTICK->CSR |= SYSTICK_CSR_TICKINT_BIT_MASK;
}

bool systick_get_parameters(uint32_t *current_value)
{
    bool zero = false;
    *current_value = SYSTICK->CVR;
    if((SYSTICK->CVR & SYSTICK_CSR_COUNTFLAG_BIT_MASK) == SYSTICK_CSR_COUNTFLAG_BIT_MASK)
    {
        zero = true;
    }
    return zero;
}

void systick_set_reload_value(uint32_t reload_value)
{
    SYSTICK->RVR = reload_value;
}

void systick_start(void)
{
    SYSTICK->CSR |= SYSTICK_CSR_ENABLE_BIT_MASK;
}

void systick_stop(void)
{
    SYSTICK->CSR &= ~SYSTICK_CSR_ENABLE_BIT_MASK;
}

void systick_initialization(struct t_systick_driver *config)
{
    if(config->systick_on_processor_clock)
    {
        SYSTICK->CSR |= SYSTICK_CSR_CLKSOURCE_BIT_MASK;
    }
    else
    {
        SYSTICK->CSR &= ~SYSTICK_CSR_CLKSOURCE_BIT_MASK;
    }

    SYSTICK->RVR = config->reload_value;

    if(config->irq.active)
    {
        SYSTICK->CSR |= SYSTICK_CSR_TICKINT_BIT_MASK;
        systick_callback = config->irq.callback;
    }
    
}

extern void SysTick_Handler(void)
{
    systick_callback();
}
