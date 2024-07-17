#include <stdint.h>

#ifndef NVIC_REGBASE_H_
#define NVIC_REGBASE_H_

#define NVIC_BASE   0xE000E100U                     /* NVIC base address in the alias region.                           */
#define NVIC        ((NVIC_Type *)NVIC_BASE)        /* NVIC_Typedef structure position in the NVIC base address.        */


/* NVIC - Size of Registers Arrays        */
#define NVIC_ISER_COUNT        8u
#define NVIC_ICER_COUNT        8u
#define NVIC_ISPR_COUNT        8u
#define NVIC_ICPR_COUNT        8u
#define NVIC_IABR_COUNT        8u
#define NVIC_IP_COUNT          240u


/* NVIC registers structure               */
typedef struct {
    volatile uint32_t ISER[NVIC_ISER_COUNT];
             uint8_t RESERVED_0[96];
    volatile uint32_t ICER[NVIC_ICER_COUNT];
             uint8_t RESERVED_1[96];
    volatile uint32_t ISPR[NVIC_ISPR_COUNT];
             uint8_t RESERVED_2[96];
    volatile uint32_t ICPR[NVIC_ICPR_COUNT];
             uint8_t RESERVED_3[96];
    volatile uint32_t IABR[NVIC_IABR_COUNT];
             uint8_t RESERVED_4[224];
    volatile uint8_t IP[NVIC_IP_COUNT];
             uint8_t RESERVED_5[2576];
    volatile uint32_t STIR;
} NVIC_Type, *NVIC_MemMapPtr;

#endif /* NVIC_REGBASE_H_ */
