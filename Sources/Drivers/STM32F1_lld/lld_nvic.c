/*
 *        lld_nvic.c
 *
 *    Created on: Nov 27, 2018
 *    Author: Stéphane Amans
 */

/* Include files        */
#include "lld_nvic.h"


void enable_nvic_irq(uint8_t irq)
{
	/* Compute the NVIC register index ( /32 because NVIC register are 32 bits wide). */
	/* Compute the bit position in the NVIC register.                                 */
    uint8_t index = irq / 32;
    uint8_t pos	= irq % 32;
    uint32_t temp_NVIC = 0x01 << pos;
    NVIC->ICPR[index] = temp_NVIC; /* Clear any pending interrupt. */
    NVIC->ISER[index] = temp_NVIC; /* Set enable register.         */
}


void disable_nvic_irq(uint8_t irq)
{
	/* Compute the NVIC register index ( /32 because NVIC register are 32 bits wide). */
	/* Compute the bit position in the NVIC register.                                 */
    uint8_t index = irq / 32;
    uint8_t pos	= irq % 32;
    uint32_t temp_NVIC = 0x01 << pos;
    NVIC->ICPR[index] = temp_NVIC; /* Clear any pending interrupt. */
    NVIC->ICER[index] = temp_NVIC; /* Clear enable register.       */
}


void clear_pending_nvic_irq(uint8_t irq)
{
	/* Compute the NVIC register index ( /32 because NVIC register are 32 bits wide). */
	/* Compute the bit position in the NVIC register.                                 */
    uint8_t index = irq / 32;
    uint8_t pos	= irq % 32;
    uint32_t temp_NVIC = 0x01 << pos; /* Set to 1 the bit number "pos". */
    NVIC->ICPR[index] |= temp_NVIC;   /* Clear any pending interrupt.   */
}


void set_nvic_priority(uint8_t irq, uint8_t priority)
{
	priority = priority << 4; /* Shift left to have priority on MSB */
	NVIC->IP[irq] = priority; /* Set the priority: priority 0-15    */
}
