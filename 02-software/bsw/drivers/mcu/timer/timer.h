/*
 * lld_timer.h
 *
 * Created on: Mar 17, 2020
 * Author: Stephane Amans
 */
#ifndef TIMER_H_
#define TIMER_H_


/* Include files:        */
#include <stdint.h>

#include "driver_common.h"
#include "clock.h"
#include "nvic.h"
#include "regbase_tim.h"

#include "soc_configuration.h"

/* MISSING: ENABLE CAPTURE INTERRUPT  */


typedef enum
{
    timer          = 0,
    input_capture  = 1,
    output_compare = 2,
    pwm_input      = 3,
    pwm_output     = 4
}timer_mode;


typedef enum
{
    frozen       = 0x00,
    active       = 0x10,
    inactive     = 0x20,
    toggle       = 0x30,
    fce_inactive = 0x40,
    fce_active   = 0x50,
    pwm1         = 0x60,
    pwm2         = 0x70
}oc_mode;


typedef enum
{
    disable = 0,
    enable  = 1
}channel_en;


typedef enum
{
    ccx_channel_output             = 0,
    ccx_channel_input_ICx_straight = 1,
    ccx_channel_input_ICx_cross    = 2,
    ccx_channel_input_ICx_on_TRC   = 3
}ic_oc_pin_mux;


typedef enum
{
    out_active_high_in_rising = 0,
    out_active_low_in_falling = 1
}ic_oc_polarity;


typedef enum
{
    irq_dma_inactive = 0,
    irq              = 2,
    dma              = 200
}irq_dma_active;

typedef enum
{
    one_pulse = 0,
    continuous = 1
}oc_repeat;


typedef struct
{
    channel_en channel_enable;
    ic_oc_pin_mux tim_mux;
    oc_repeat repeat;

    timer_mode timer_mode;
    oc_mode out_cmp_mode;
    uint8_t percent_pwm;

    ic_oc_polarity polarity;
    struct
    {
        irq_dma_active active;
//        uint8_t edge;
        uint8_t priority;
//        void(*callback)(uint32_t);
    }irq_dma;
}t_timer_params;


struct t_timer_cfg
{
    TIM_TypeDef *timer;
    uint32_t time_frequency;
    t_timer_params channel[4];
};





/* Functions prototypes:                       */

/** Configure timer (port, pin, in/out, mode, interruption):
 *
 * \param timer: Pointer to the timer register base.
 * \param cfg: Configuration structure.
 *
 * \return: Error code or 0 if OK.
 *
 */
t_error_handling timer_init(struct t_timer_cfg *cfg);

/** Start timer.
 *
 * \param timer: Address of the timer.
 *
 * \return: Error code or OK.
 *
 */
t_error_handling tim_start(TIM_TypeDef *timer);


/** Stop timer.
 *
 * \param timer: Address of the timer.
 *
 * \return: Error code or OK.
 *
 */
t_error_handling tim_stop(TIM_TypeDef *timer);


/** Wait timer end by polling.
 *
 * \param timer: Address of the timer.
 * \param channel: Number of the channel to start.
 *
 * \return: Error code or OK.
 *
 */
t_error_handling tim_wait(TIM_TypeDef *timer, uint8_t channel);


/** Wait for timer input capture acquisition.
 *
 * \param timer: Address of the timer.
 * \param channel: Number of the channel to get.
 *
 * \return : measured capture value.
 *
 */
uint16_t tim_wait_input_capture(TIM_TypeDef *timer, uint8_t channel);



/* Pointer callback function table prototype	*/
//void (*tim1_callback[4])(uint32_t pin_numb);


#endif /* TIMER_H_ */
