#include <stdint.h>

#ifndef FLASH_REGBASE_H_
#define FLASH_REGBASE_H_

#define PERIPH_BASE        0x40000000U                            /* Peripheral base address in the alias region.                       */
#define AHBPERIPH_BASE     (PERIPH_BASE + 0x00020000U)            /* AHB peripheral base address in the alias region.                   */
#define FLASH_R_BASE       (AHBPERIPH_BASE + 0x00002000U)         /* Flash Peripheral base address in the alias region.                 */
#define FLASH              ((FLASH_TypeDef *)FLASH_R_BASE)        /* FLASH_Typedef structure position in the flash base address.        */


/* FLASH registers structure        */
typedef struct
{
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    volatile uint32_t RESERVED;
    volatile uint32_t OBR;
    volatile uint32_t WRPR;
} FLASH_TypeDef;

#endif /* FLASH_REGBASE_H_ */
