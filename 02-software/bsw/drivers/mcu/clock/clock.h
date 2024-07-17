/*
 * lld_clock.h
 *
 * Created on: Apr 20, 2019
 * Author: Stephane Amans
 */
#ifndef LLD_CLOCK_H_
#define LLD_CLOCK_H_

/* Include files:        */
#include <stdint.h>

#include "error.h"

#include "driver_common.h"

/* Clock units defines        */
#define  HZ  *1
#define KHZ  *1000
#define MHZ  *1000000

#define  SEC  *1000000000
#define MSEC  *1000000
#define USEC  *1000

/* Source selection enumeration definition:        */
enum t_clock_sources
{
    HSI_OSC     = 1,  /* High speed internal oscillator mask.    */
    SYSCLOCK    = 2,  /* System clock ID for output.             */
    HSE_OSC     = 3,  /* High speed external oscillator mask.    */
    HSE_BYP     = 4,  /* High speed external drive source mask.  */
    PLL_HSI     = 5,  /* PLL clocked with HSI oscillator.        */
    PLL_HSE     = 6,  /* PLL clocked with HSE oscillator.        */
    PLL_HSE_BYP = 7   /* PLL clocked with HSE Bypass oscillator. */
};

/* Clock initialization structure definition :        */
struct t_clock_config
{
    uint32_t sys_clk_freq;
    uint32_t AHB_clk_freq;
    uint32_t APB1_clk_freq;
    uint32_t APB2_clk_freq;
    enum t_clock_sources source;
};

/* Clock driver structure definition :        */
struct t_clock_driver
{
    uint32_t sys_clk_freq;
    uint32_t AHB_clk_freq;
    uint32_t APB1_clk_freq;
    uint32_t APB2_clk_freq;
    enum t_clock_sources source;
};

/* Extern driver declaration */
extern struct t_clock_driver clock_driver;


/* Functions prototypes:        */

/** Configure all SOC clocks.
 *
 * \param config: Pointer to the configuration structure.
 * \param driver: Pointer to the driver structure.
 *
 * \return: Error code or ERROR_OK.
 *
 */
t_error_handling clock_init(struct t_clock_driver *driver, struct t_clock_config *config);


/** Disable the peripherak clock.
 *
 * \param peripheral: enum of the peripheral.
 *
 */
void disable_clock(enum t_peripheral peripheral);


/** Enable the peripherak clock.
 *
 * \param peripheral: enum of the peripheral.
 *
 * \return: Error code or ERROR_OK.
 *
 */
t_error_handling enable_clock(enum t_peripheral peripheral);


/** Get clock driver.
 *
 * \param: void
 *
 * \return Pointer to the driver address structure
 *
 */
struct t_clock_driver *get_clock_driver(void);


/** Select the clock to output on the dedicated pin.
 *
 * \param source: Source clock to output.
 *
 */
void output_clock(enum t_clock_sources source);

void reset_module(enum t_peripheral peripheral);

#endif /* LLD_CLOCK_H_ */
