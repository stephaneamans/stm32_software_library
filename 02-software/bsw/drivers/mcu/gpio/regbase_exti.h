#include <stdint.h>

#ifndef EXTI_REGBASE_H_
#define EXTI_REGBASE_H_

/* EXTI registers structure        */
struct t_exti_regs
{
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
};

#endif /* EXTI_REGBASE_H_ */
