#include <stdint.h>

#ifndef USART_REGBASE_H_
#define USART_REGBASE_H_

/* USART registers structure        */
struct t_usart_regs
{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
};

#endif /* USART_REGBASE_H_ */
