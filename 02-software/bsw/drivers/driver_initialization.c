/*
 * driver_initialization.c
 *
 * Created on: Aug 21, 2022
 * Author: Stephane Amans
 *
 */

/* Include files.        */
#include "driver_initialization.h"

void soc_configuration(void)
{
    clock_init(&clock_driver, &clock_config);
    INIT_SYSTICK()
    INIT_DMA()
    INIT_GPTO()
    INIT_SPI()
    INIT_USART()
}

void platform_configuration(void)
{

}