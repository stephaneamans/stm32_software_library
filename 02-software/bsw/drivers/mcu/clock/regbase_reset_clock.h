#include <stdint.h>

#ifndef RESET_CLOCK_REGBASE_H_
#define RESET_CLOCK_REGBASE_H_

/* RCC registers structure        */
typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
}t_reset_clock_reg;

#define RCC ((t_reset_clock_reg *)RCC_BASE)

#endif /* RESET_CLOCK_REGBASE_H_ */
