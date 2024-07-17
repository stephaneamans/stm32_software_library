#include <stdint.h>

#ifndef AFIO_REGBASE_H_
#define AFIO_REGBASE_H_

/* AFIO registers structure         */
struct t_afio_regs
{
    volatile uint32_t EVCR;
    volatile uint32_t MAPR;
    volatile uint32_t EXTICR[4];
    volatile uint32_t RESERVED0;
    volatile uint32_t MAPR2;
};

#endif /* AFIO_REGBASE_H_ */
