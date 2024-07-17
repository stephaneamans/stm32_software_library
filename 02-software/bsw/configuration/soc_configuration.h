#ifndef CONFIGURATION_SOC_H
#define CONFIGURATION_SOC_H

#include "afio.h"
#include "clock.h"
#include "dma.h"
#include "exti.h"
#include "spi.h"

#include "gpio.h"
#include "systick.h"
#include "usart.h"
#include "timer.h"

/*********************************
 * Clock definitions
 ********************************/

/* SOC parameters:   */

#define MAX_SPI1_PERIPHERALS       2
#define MAX_SPI2_PERIPHERALS       2

#define SYSTICK_PERIOD_NS          28

#define CLOCK_FREQ_MHZ 8 MHZ

#define GPIO_PIN_NUMBER         37
#define EXTI_CHANNEL_NUMBER     5
#define USART_IP_NUMBER         2
#define DMA_IP_NUMBER           1
#define DMA_CHANNEL_NUMBER      7
#define TIMER_IP_NUMBER         3
#define TIMER_CHANNELS_NUMBER   4
#define SPI_IP_NUMBER           2

/* Register map         */
#define PERIPH_BASE        0x40000000U
#define AHBPERIPH_BASE     (PERIPH_BASE + 0x00020000U)
#define APB1PERIPH_BASE    (PERIPH_BASE)
#define APB2PERIPH_BASE    (PERIPH_BASE + 0x00010000U)
#define RCC_BASE           (AHBPERIPH_BASE + 0x00001000U)

/* NVIC interrupt offset */
#define NVIC_WWDG_OFFSET      0
#define NVIC_PVD_OFFSET       1
#define NVIC_TAMPER_OFFSET    2
#define NVIC_RTC_OFFSET       3
#define NVIC_FLASH_OFFSET     4
#define NVIC_RCC_OFFSET       5
#define NVIC_GPIO_OFFSET      6
#define NVIC_DMA1_OFFSET      11
#define NVIC_ADC1_OFFSET      18
#define NVIC_CAN1_OFFSET      19
#define NVIC_EXTI9_5_OFFSET   23
#define NVIC_TIM1_OFFSET      24
#define NVIC_TIM2_OFFSET      28
#define NVIC_TIM3_OFFSET      29
#define NVIC_TIM4_OFFSET      30
#define NVIC_I2C_OFFSET       31
#define NVIC_SPI_OFFSET       35
#define NVIC_USART_OFFSET     37

extern struct t_clock_config clock_config;

extern struct t_systick_driver systick;

extern struct t_gpio_driver pa0;
extern struct t_gpio_driver pa1;
extern struct t_gpio_driver pa2;
extern struct t_gpio_driver pa3;
extern struct t_gpio_driver pa4;
extern struct t_gpio_driver pa5;
extern struct t_gpio_driver pa6;
extern struct t_gpio_driver pa7;
extern struct t_gpio_driver pa8;
extern struct t_gpio_driver pa9;
extern struct t_gpio_driver pa10;
extern struct t_gpio_driver pa11;
extern struct t_gpio_driver pa12;
extern struct t_gpio_driver pa13;
extern struct t_gpio_driver pa14;
extern struct t_gpio_driver pa15;
extern struct t_gpio_driver pb0;
extern struct t_gpio_driver pb1;
extern struct t_gpio_driver pb2;
extern struct t_gpio_driver pb3;
extern struct t_gpio_driver pb4;
extern struct t_gpio_driver pb5;
extern struct t_gpio_driver pb6;
extern struct t_gpio_driver pb7;
extern struct t_gpio_driver pb8;
extern struct t_gpio_driver pb9;
extern struct t_gpio_driver pb10;
extern struct t_gpio_driver pb11;
extern struct t_gpio_driver pb12;
extern struct t_gpio_driver pb13;
extern struct t_gpio_driver pb14;
extern struct t_gpio_driver pb15;
extern struct t_gpio_driver pc13;
extern struct t_gpio_driver pc14;
extern struct t_gpio_driver pc15;
extern struct t_gpio_driver pd0;
extern struct t_gpio_driver pd1;

extern struct t_dma_driver dma1;
extern struct t_dma_channel_driver dma_channel1;
extern struct t_dma_channel_driver dma_channel2;
extern struct t_dma_channel_driver dma_channel3;
extern struct t_dma_channel_driver dma_channel4;
extern struct t_dma_channel_driver dma_channel5;
extern struct t_dma_channel_driver dma_channel6;
extern struct t_dma_channel_driver dma_channel7;

extern struct t_spi_driver spi1;
extern struct t_spi_driver spi2;

extern struct t_usart_driver usart1;
extern struct t_usart_driver usart2;

#define INIT_SYSTICK()             \
                                   \
systick_initialization(&systick);  \
                                   \

#define INIT_GPTO()        \
gpio_initialization(&pa0);  \
gpio_initialization(&pa1);  \
gpio_initialization(&pa2);  \
gpio_initialization(&pa3);  \
gpio_initialization(&pa4);  \
gpio_initialization(&pa5);  \
gpio_initialization(&pa6);  \
gpio_initialization(&pa7);  \
gpio_initialization(&pa8);  \
gpio_initialization(&pa9);  \
gpio_initialization(&pa10);  \
gpio_initialization(&pa11);  \
gpio_initialization(&pa12);  \
gpio_initialization(&pa13);  \
gpio_initialization(&pa14);  \
gpio_initialization(&pa15);  \
gpio_initialization(&pb0);  \
gpio_initialization(&pb1);  \
gpio_initialization(&pb2);  \
gpio_initialization(&pb3);  \
gpio_initialization(&pb4);  \
gpio_initialization(&pb5);  \
gpio_initialization(&pb6);  \
gpio_initialization(&pb7);  \
gpio_initialization(&pb8);  \
gpio_initialization(&pb9);  \
gpio_initialization(&pb10);  \
gpio_initialization(&pb11);  \
gpio_initialization(&pb12);  \
gpio_initialization(&pb13);  \
gpio_initialization(&pb14);  \
gpio_initialization(&pb15);  \
gpio_initialization(&pc13);  \
gpio_initialization(&pc14);  \
gpio_initialization(&pc15);  \
gpio_initialization(&pd0);  \
gpio_initialization(&pd1);  \
                           \

#define INIT_DMA()                        \
dma_initialization(&dma1, &dma_channel1); \
dma_initialization(&dma1, &dma_channel2); \
dma_initialization(&dma1, &dma_channel3); \
dma_initialization(&dma1, &dma_channel4); \
dma_initialization(&dma1, &dma_channel5); \
dma_initialization(&dma1, &dma_channel6); \
dma_initialization(&dma1, &dma_channel7); \
                                          \


#define INIT_SPI()                 \
                                   \
spi_initialization(&spi1);          \
spi_initialization(&spi2);          \
                                   \


#define INIT_USART()               \
                                   \
usart_initialization(&usart1);      \
usart_initialization(&usart2);      \
                                   \

#endif /* CONFIGURATION_SOC_H_ */
