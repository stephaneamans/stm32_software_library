#include <stdint.h>

#ifndef TIM_REGBASE_H_
#define TIM_REGBASE_H_

#define PERIPH_BASE        0x40000000U                            /* Peripheral base address in the alias region.                       */
#define APB1PERIPH_BASE    (PERIPH_BASE)                          /* APB1 peripheral base address in the alias region.                   */
#define APB2PERIPH_BASE    (PERIPH_BASE + 0x00010000U)            /* APB2 peripheral base address in the alias region.                  */

#define TIM1_BASE          (APB2PERIPH_BASE + 0x00002C00U)        /* TIM1 Peripheral base address in the alias region.                  */
#define TIM2_BASE          (APB1PERIPH_BASE + 0x00000000U)        /* TIM2 Peripheral base address in the alias region.                  */
#define TIM3_BASE          (APB1PERIPH_BASE + 0x00000400U)        /* TIM3 Peripheral base address in the alias region.                  */
#define TIM1               ((TIM_TypeDef *)TIM1_BASE)             /* TIM_Typedef structure position in the TIM base address.            */
#define TIM2               ((TIM_TypeDef *)TIM2_BASE)             /* TIM_Typedef structure position in the TIM base address.            */
#define TIM3               ((TIM_TypeDef *)TIM3_BASE)             /* TIM_Typedef structure position in the TIM base address.            */


/* FLASH registers structure        */
typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
    volatile uint32_t OR;
}TIM_TypeDef;

#endif /* TIM_REGBASE_H_ */
