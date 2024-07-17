/*
 * lld_nvic.h
 *
 * Created on: Nov 27, 2018
 * Author: Stephane Amans
 */
#ifndef LLD_NVIC_H_
#define LLD_NVIC_H_


/* Include files:        */
#include <stdint.h>

#include "error.h"

/* Interruptions number enumeration definition:        */
enum irq_id{
    // Reserved
    // Reset_Handler        Priority -3
    // NMI_Handler          Priority -2
    // HardFault_Handler    Priority -1
    // MemManage_Handler
    // BusFault_Handler
    // UsageFault_Handler
	// Reserved
    // Reserved
    // Reserved
    // Reserved
    // SVC_Handler
    // DebugMon_Handler
    // Reserved
    // PendSV_Handler
    // SysTick_Handler
    IRQ_WWDG              = 0,     /* Window Watchdog interrupt                        */
    IRQ_PVD               = 1,     /* PVD through EXTI line detection interrupt        */
    IRQ_TAMPER            = 2,     /* Tamper interrupt                                 */
    IRQ_RTC               = 3,     /* RTC global interrupt                             */
    IRQ_FLASH             = 4,     /* Flash global interrupt                           */
    IRQ_RCC               = 5,     /* RCC global interrupt                             */
    IRQ_EXTI_0            = 6,     /* EXTI Line0 interrupt                             */
    IRQ_EXTI_1            = 7,     /* EXTI Line1 interrupt                             */
    IRQ_EXTI_2            = 8,     /* EXTI Line2 interrupt                             */
    IRQ_EXTI_3            = 9,     /* EXTI Line3 interrupt                             */
    IRQ_EXTI_4            = 10,    /* EXTI Line4 interrupt                             */
    IRQ_DMA1_CHANNEL1     = 11,    /* DMA1 Channel1 global interrupt                   */
    IRQ_DMA1_CHANNEL2     = 12,    /* DMA1 Channel2 global interrupt                   */
    IRQ_DMA1_CHANNEL3     = 13,    /* DMA1 Channel3 global interrupt                   */
    IRQ_DMA1_CHANNEL4     = 14,    /* DMA1 Channel4 global interrupt                   */
    IRQ_DMA1_CHANNEL5     = 15,    /* DMA1 Channel5 global interrupt                   */
    IRQ_DMA1_CHANNEL6     = 16,    /* DMA1 Channel6 global interrupt                   */
    IRQ_DMA1_CHANNEL7     = 17,    /* DMA1 Channel7 global interrupt                   */
    IRQ_ADC1_2            = 18,    /* ADC1 and ADC2 global interrupt                   */
    IRQ_USB_HP_CAN_TX     = 19,    /* USB High Priority or CAN TX interrupts           */
    IRQ_USB_LP_CAN_RX0    = 20,    /* USB Low Priority or CAN RX0 interrupts           */
    IRQ_CAN_RX1           = 21,    /* CAN RX1 interrupt                                */
    IRQ_CAN_SCE           = 22,    /* CAN SCE interrupt                                */
    IRQ_EXTI9_5           = 23,    /* EXTI Line[9:5] interrupts                        */
    IRQ_TIM1_BRK          = 24,    /* TIM1 Break interrupt                             */
    IRQ_TIM1_UP           = 25,    /* TIM1 Update interrupt                            */
    IRQ_TIM1_TRG_COM      = 26,    /* TIM1 Trigger and Commutation interrupts          */
    IRQ_TIM1_CC           = 27,    /* TIM1 Capture Compare interrupt                   */
    IRQ_TIM2              = 28,    /* TIM2 global interrupt                            */
    IRQ_TIM3              = 29,    /* TIM3 global interrupt                            */
    IRQ_TIM4              = 30,    /* TIM4 global interrupt                            */
    IRQ_I2C1_EV           = 31,    /* I2C1 event interrupt                             */
    IRQ_I2C1_ER           = 32,    /* I2C1 error interrupt                             */
    IRQ_I2C2_EV           = 33,    /* I2C2 event interrupt                             */
    IRQ_I2C2_ER           = 34,    /* I2C2 error interrupt                             */
    IRQ_SPI1              = 35,    /* SPI1 global interrupt                            */
    IRQ_SPI2              = 36,    /* SPI2 global interrupt                            */
    IRQ_USART_1           = 37,    /* USART1 global interrupt                          */
    IRQ_USART_2           = 38,    /* USART2 global interrupt                          */
    IRQ_USART_3           = 39,    /* USART3 global interrupt                          */
    IRQ_EXTI15_10         = 40,    /* EXTI Line[15:10] interrupts                      */
    IRQ_RTCAlarm          = 41,    /* RTC Alarms through EXTI line interrupt           */
    // Reserved           = 42,    /* Reserved                                         */
    IRQ_IM8_BRK           = 43,    /* TIM8 Break interrupt                             */
    IRQ_IM8_UP            = 44,    /* TIM8 Update interrupt                            */
    IRQ_IM8_TRG_COM	      = 45,    /* TIM8 Trigger and Commutation interrupts          */
    IRQ_TIM8_CC           = 46,    /* TIM8 Capture Compare interrupt                   */
    IRQ_ADC3              = 47,    /* ADC3 global interrupt                            */
    IRQ_FSMC              = 48,    /* FSMC global interrupt                            */
    IRQ_SDIO              = 49,    /* SDIO global interrupt                            */
    IRQ_TIM5              = 50,    /* TIM5 global interrupt                            */
    IRQ_SPI3              = 51,    /* SPI3 global interrupt                            */
    IRQ_UART4             = 52,    /* UART4 global interrupt                           */
    IRQ_UART5             = 53,    /* UART5 global interrupt                           */
    IRQ_TIM6              = 54,    /* TIM6 global interrupt                            */
    IRQ_TIM7              = 55,    /* TIM7 global interrupt                            */
    IRQ_DMA2_Channel1     = 56,    /* DMA2 Channel1 global interrupt                   */
    IRQ_DMA2_Channel2     = 57,    /* DMA2 Channel2 global interrupt                   */
    IRQ_DMA2_Channel3     = 58,    /* DMA2 Channel3 global interrupt                   */
    IRQ_DMA2_Channel4_5   = 59     /* DMA2 Channel4 and DMA2 Channel5 global interrupt */
};


/* Interruptions priority enumeration definition:        */
typedef enum irq_priority
{
    prio0 = 0,    /* Priority level 0.    */
    prio1 = 1,    /* Priority level 1.    */
    prio2 = 2,    /* Priority level 2.	  */
    prio3 = 3,    /* Priority level 3.	  */
    prio4 = 4,	  /* Priority level 4.	  */
    prio5 = 5,	  /* Priority level 5.	  */
    prio6 = 6,    /* Priority level 6.    */
    prio7 = 7,    /* Priority level 7.    */
    prio8 = 8,    /* Priority level 8.    */
    prio9 = 9,    /* Priority level 9.    */
    prio10 = 10,  /* Priority level 10.   */
    prio11 = 11,  /* Priority level 11.   */
    prio12 = 12,  /* Priority level 12.   */
    prio13 = 13,  /* Priority level 13.   */
    prio14 = 14,  /* Priority level 14.   */
    prio15 = 15,  /* Priority level 15.   */
}irq_priority;


/* Functions prototypes        */

/** Set the NVIC register to unmask the IRQ
*
* \parameter irq: Interruption number.
*
* \return: Error code or 0 if ERROR_OK.
*
*/
t_error_handling enable_nvic_irq(uint8_t irq);


/** Set the NVIC register to mask the IRQ
*
* \param irq: Interruption number.
*
* \return: Error code or 0 if ERROR_OK.
*
*/
t_error_handling disable_nvic_irq(uint8_t irq);


/** Clear the NVIC pending IRQ
*
* \param irq: Interruption number.
*
* \return: Error code or 0 if ERROR_OK.
*
*/
t_error_handling clear_pending_nvic_irq(uint8_t irq);


/** Set the NVIC priority register to prioritize the IRQ.
*
* \param irq: Interruption number.
* \param priority: Interruption priority.
*
* \return: Error code or 0 if ERROR_OK.
*
*/
t_error_handling set_nvic_priority(uint8_t irq, uint8_t priority);

#endif /* LLD_NVIC */
