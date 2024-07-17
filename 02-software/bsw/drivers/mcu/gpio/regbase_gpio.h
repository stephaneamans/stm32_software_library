#include <stdint.h>

#ifndef GPIO_REGBASE_H_
#define GPIO_REGBASE_H_

/* GPIO registers structure        */
struct t_gpio_regs
{
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
};

#endif /* GPIO_REGBASE_H_ */
