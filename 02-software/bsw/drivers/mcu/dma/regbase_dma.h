#include <stdint.h>

#ifndef DMA_REGBASE_H_
#define DMA_REGBASE_H_

/* DMA registers structure        */
struct t_dma_regs
{
    volatile uint32_t ISR;
    volatile uint32_t IFCR;
};

/* DMA channel registers structure         */
struct t_dma_channel_regs
{
    volatile uint32_t CCR;
    volatile uint32_t CNDTR;
    volatile uint32_t CPAR;
    volatile uint32_t CMAR;
};

#endif /* DMA_REGBASE_H_ */
