/*
 * lld_timer.c
 *
 * Created on: Mar 17, 2020
 * Author: Stephane Amans
 */

/* Include files.        */
#include "timer.h"


/** Enable an TIM port clock.
 *
 * \param timer: address of the TIM entity to clock.
 *
 * \return: Error code or OK.
 *
 */
static t_error_handling timer_enable_clock(TIM_TypeDef *timer);
/*
##### How to use this driver #####
==============================================================================
[..]
(#) Initialize the TIM low level resources by implementing the following functions
   depending from feature used :
     (++) Time Base : HAL_TIM_Base_MspInit()
     (++) Input Capture : HAL_TIM_IC_MspInit()
     (++) Output Compare : HAL_TIM_OC_MspInit()
     (++) PWM generation : HAL_TIM_PWM_MspInit()
     (++) One-pulse mode output : HAL_TIM_OnePulse_MspInit()
     (++) Encoder mode output : HAL_TIM_Encoder_MspInit()

(#) Initialize the TIM low level resources :
  (##) Enable the TIM interface clock using __HAL_RCC_TIMx_CLK_ENABLE();
  (##) TIM pins configuration
      (+++) Enable the clock for the TIM GPIOs using the following function:
       __HAL_RCC_GPIOx_CLK_ENABLE();
      (+++) Configure these TIM pins in Alternate function mode using HAL_GPIO_Init();

(#) The external Clock can be configured, if needed (the default clock is the
   internal clock from the APBx), using the following function:
   HAL_TIM_ConfigClockSource, the clock configuration should be done before
   any start function.

(#) Configure the TIM in the desired functioning mode using one of the
 Initialization function of this driver:
 (++) HAL_TIM_Base_Init: to use the Timer to generate a simple time base
 (++) HAL_TIM_OC_Init and HAL_TIM_OC_ConfigChannel: to use the Timer to generate an
      Output Compare signal.
 (++) HAL_TIM_PWM_Init and HAL_TIM_PWM_ConfigChannel: to use the Timer to generate a
      PWM signal.
 (++) HAL_TIM_IC_Init and HAL_TIM_IC_ConfigChannel: to use the Timer to measure an
      external signal.
   (++) HAL_TIM_OnePulse_Init and HAL_TIM_OnePulse_ConfigChannel: to use the Timer
        in One Pulse Mode.
 (++) HAL_TIM_Encoder_Init: to use the Timer Encoder Interface.

(#) Activate the TIM peripheral using one of the start functions depending from the feature used:
     (++) Time Base : HAL_TIM_Base_Start(), HAL_TIM_Base_Start_DMA(), HAL_TIM_Base_Start_IT()
     (++) Input Capture :  HAL_TIM_IC_Start(), HAL_TIM_IC_Start_DMA(), HAL_TIM_IC_Start_IT()
     (++) Output Compare : HAL_TIM_OC_Start(), HAL_TIM_OC_Start_DMA(), HAL_TIM_OC_Start_IT()
     (++) PWM generation : HAL_TIM_PWM_Start(), HAL_TIM_PWM_Start_DMA(), HAL_TIM_PWM_Start_IT()
     (++) One-pulse mode output : HAL_TIM_OnePulse_Start(), HAL_TIM_OnePulse_Start_IT()
     (++) Encoder mode output : HAL_TIM_Encoder_Start(), HAL_TIM_Encoder_Start_DMA(), HAL_TIM_Encoder_Start_IT().

(#) The DMA Burst is managed with the two following functions:
   HAL_TIM_DMABurst_WriteStart()
   HAL_TIM_DMABurst_ReadStart()

@endverbatim
*/

/** Mes notes:
 * voir bits CKD TIMX_CR1
 * voir bit ARPE TIMX_CR1
 *
 * Select the active input: TIMx_CCR1 must be linked to the TI1 input, so write the CC1S bits to 01 in the TIMx_CCMR1 register.
 * As soon as CC1S becomes different from 00,the channel is configured in input and the TIMx_CCR1 register becomes read-only.
 * Program the input prescaler.
 * Program the input filter with respect to the signal connected to the timer (programming ICxF bits in the TIMx_CCMRx register if the input is a TIx input). Filter duration = 8 clock cycles.
 * These bits can not be modified as long as LOCK level 3 has been programmed (LOCK bits in TIMx_BDTR register) and CC1S=’00’ (the channel is configured in output)
 * Select the counter clock (internal, external, prescaler).
 * The timer is able to generate PWM in edge-aligned mode or center-aligned mode depending on the CMS bits in the TIMx_CR1 register.



PWM input mode
This mode is a particular case of input capture mode. The procedure is the same except:
• Two ICx signals are mapped on the same TIx input.
• These 2 ICx signals are active on edges with opposite polarity.
• One of the two TIxFP signals is selected as trigger input and the slave mode controller is configured in reset mode.

For example, user can measure the period (in TIMx_CCR1 register) and the duty cycle (in
TIMx_CCR2 register) of the PWM applied on TI1 using the following procedure (depending
on CK_INT frequency and prescaler value):
• Select the active input for TIMx_CCR1: write the CC1S bits to 01 in the TIMx_CCMR1 register (TI1 selected).
• Select the active polarity for TI1FP1 (used both for capture in TIMx_CCR1 and counter clear): write the CC1P bit to ‘0’ (active on rising edge).
• Select the active input for TIMx_CCR2: write the CC2S bits to 10 in the TIMx_CCMR1 register (TI1 selected).
• Select the active polarity for TI1FP2 (used for capture in TIMx_CCR2): write the CC2P bit to ‘1’ (active on falling edge).
• Select the valid trigger input: write the TS bits to 101 in the TIMx_SMCR register (TI1FP1 selected).
• Configure the slave mode controller in reset mode: write the SMS bits to 100 in the TIMx_SMCR register.
• Enable the captures: write the CC1E and CC2E bits to ‘1’ in the TIMx_CCER register.


PWM edge-aligned mode
Upcounting configuration
Upcounting is active when the DIR bit in the TIMx_CR1 register is low. Refer to Upcounting mode.
In the following example, we consider PWM mode 1. The reference PWM signal OCxREF is high as long as TIMx_CNT < TIMx_CCRx else it becomes low. 
If the compare value in TIMx_CCRx is greater than the auto-reload value (in TIMx_ARR) then OCxREF is held at ‘1’. 
If the compare value is 0 then OCxRef is held at ‘0’.
Figure 84 shows some edge-aligned PWM waveforms in an example where TIMx_ARR=8.

Downcounting configuration
Downcounting is active when DIR bit in TIMx_CR1 register is high. Refer to Downcounting mode
In PWM mode 1, the reference signal OCxRef is low as long as TIMx_CNT > TIMx_CCRx else it becomes high. 
If the compare value in TIMx_CCRx is greater than the auto-reload value in TIMx_ARR, then OCxREF is held at ‘1’. 0% PWM is not possible in this mode.

PWM center-aligned mode
Center-aligned mode is active when the CMS bits in TIMx_CR1 register are different from
‘00’ (all the remaining configurations having the same effect on the OCxRef/OCx signals).
The compare flag is set when the counter counts up, when it counts down or both when it
counts up and down depending on the CMS bits configuration. The direction bit (DIR) in the
TIMx_CR1 register is updated by hardware and must not be changed by software. Refer to
Center-aligned mode (up/down counting).
Figure 85 shows some center-aligned PWM waveforms in an example where:
• TIMx_ARR=8,
• PWM mode is the PWM mode 1,
• The flag is set when the counter counts down corresponding to the center-aligned
mode 1 selected for CMS=01 in TIMx_CR1 register.
Hints on using center-aligned mode:
• When starting in center-aligned mode, the current up-down configuration is used. It
means that the counter counts up or down depending on the value written in the DIR bit
in the TIMx_CR1 register. Moreover, the DIR and CMS bits must not be changed at the
same time by the software.
• Writing to the counter while running in center-aligned mode is not recommended as it
can lead to unexpected results. In particular:
•
320/1134
– The direction is not updated if the user writes a value in the counter greater than
the auto-reload value (TIMx_CNT>TIMx_ARR). For example, if the counter was
counting up, it will continue to count up.
– The direction is updated if the user writes 0 or write the TIMx_ARR value in the
counter but no Update Event UEV is generated.
The safest way to use center-aligned mode is to generate an update by software

(setting the UG bit in the TIMx_EGR register) just before starting the counter and not to
write the counter while it is running.



*/

static t_timer_cfg tim_driver[3];


   t_error_handling timer_init(struct t_timer_cfg *cfg)
{
    timer_enable_clock(cfg->timer);
    uint32_t clock = get_apb2_clock();
    uint32_t apb2_period_ns = 0;
    uint16_t prescaler = 1;
    uint32_t autoreload = 0;
    uint8_t channel_index = 0;

    apb2_period_ns = ((1000 MHZ)/ clock);
       
    if(get_apb2_clock() != get_ahb_clock())
    {
        apb2_period_ns /= 2;
    }

    autoreload = cfg->time_frequency / apb2_period_ns;

    //while((autoreload / (prescaler)) > 0xFFFF)
    //{
    //    prescaler++;
    //}

    //if(autoreload > 0xFFFF)
        while((prescaler * 0x7FFF) < autoreload)
	    {
		    prescaler++;
	    }
   // }


    //cfg->timer->PSC = prescaler + 1;
    //cfg->timer->ARR =	 (uint16_t)(autoreload / (prescaler - 1));

    //autoreload = 62414;
    //prescaler = 6;

    cfg->timer->PSC = prescaler - 1;

    if(prescaler % 2 != 0)
    {
    	cfg->timer->ARR = (uint16_t)((autoreload * 2) / prescaler);
    }
    else
    {
    	cfg->timer->ARR = (uint16_t)(autoreload / (prescaler / 2));
    }

    //float presc = (float)prescaler / 2;


    uint8_t timer_index = 0;
    if(cfg->timer == TIM2)
    {
        timer_index = 1;
    }
    else if(cfg->timer == TIM3)
    {
        timer_index = 2;
    }

   
    /* Enables interruptions and /or DMA. */
//    cfg->timer->DIER |= cfg->irq_dma.active << cfg->channel;

    /* Configure input and/or output polarity. */
//    cfg->timer->CCER = 0x00;
//    cfg->timer->CCER |= cfg->polarity << ((cfg->channel + 1) * 4);
    while(channel_index < 4)
    { 
        if(cfg->channel[channel_index].channel_enable == disable)
        {
        	channel_index++;
        }
        else
        {
             tim_driver[timer_index].timer = cfg->timer;
//           tim_driver[timer_index].mode = cfg->mode;
//           tim_driver[timer_index].tim_mux = cfg->tim_mux;
//           tim_driver[timer_index].polarity = cfg->polarity;
//           tim_driver[timer_index].prescaler = cfg->prescaler;
//           tim_driver[timer_index].irq_dma.active = cfg->irq_dma.active;

            if(cfg->channel[channel_index].timer_mode == input_capture || cfg->channel[channel_index].timer_mode == pwm_input)
            {
                if(channel_index < 2)
                {
                    cfg->timer->CCMR1 |= cfg->channel[channel_index].tim_mux << (channel_index * 8);/* TIM pin muxing configuration. */
                }
                else
                {
                    cfg->timer->CCMR2 |= (cfg->channel[channel_index].tim_mux << ((channel_index - 2) * 8)); /* TIM pin muxing configuration. */
                }
            //    cfg->timer->CCER |= cfg->polarity << ((cfg->channel + 1) * 4);

            //    cfg->timer->CCER |= 2 << (cfg->channel * 4);
                cfg->timer->CCER |= 1 << (channel_index * 4);            /* Set CCxE = 1, channel x output active. */
//            cfg->timer->CCMR1 = cfg->channel;
//        /* Enable interrupts + NVIC or DMA*/
            }

            else if(cfg->channel[channel_index].timer_mode == output_compare || cfg->channel[channel_index].timer_mode == pwm_output)
            {
            	cfg->timer->CCR1 = (uint16_t)((cfg->timer->ARR) * (100 - cfg->channel[channel_index].percent_pwm) / 100);

                cfg->timer->BDTR |= 0x8800; /* Enables master output and configure inactive state. */
                cfg->timer->RCR = 2;
    	        if(channel_index < 2)
                {
                    cfg->timer->CCMR1 &= (~0x3) << (channel_index * 8);                                       /* Clear CCxS = channel x output  */
                    cfg->timer->CCMR1 &= ~(cfg->channel[channel_index].out_cmp_mode  << (channel_index * 4)); /* Clear OCxM before setting.     */
                    cfg->timer->CCMR1 |= cfg->channel[channel_index].out_cmp_mode  << (channel_index * 4);    /* Set OCxM = channel x out mode. */
//                    if(cfg->repeat == one_pulse)
//                    {
//                        cfg->timer->CCMR1 &= (~0x0004) << (cfg->channel * 4); /* Clear OCxPE = preload reg disable.    */
//                        cfg->timer->EGR &= (~0x01); /* Clear the UG bit. */
//                    }
//                    else
//                    {
//                        cfg->timer->CCMR1 |= 0x0004 << (cfg->channel * 4);    /* Set OCxPE = preload reg enable.       */
//                        cfg->timer->EGR |= 0x01; /* Set the UG bit. */
//                    }
                }
                else
                {
                    cfg->timer->CCMR2 &= (~0x3) << ((channel_index - 2) * 8);                                       /* Clear CCxS = channel x output  */
                    cfg->timer->CCMR2 &= ~(cfg->channel[channel_index].out_cmp_mode  << ((channel_index - 2) * 4)); /* Clear OCxM before setting.     */
                    cfg->timer->CCMR2 |= cfg->channel[channel_index].out_cmp_mode  << ((channel_index - 2) * 4);    /* Set OCxM = channel x out mode. */
//                    if(cfg->repeat == one_pulse)
//                    {
//                        cfg->timer->CCMR2 &= (~0x0004) << (cfg->channel * 4); /* Clear OCxPE = preload reg disable.    */
//                    }
//                    else
//                    {
//                        cfg->timer->CCMR2 |= 0x0004 << (cfg->channel * 4);    /* Set OCxPE = preload reg enable.       */
//                    }
                }
    	        cfg->timer->CCER |= 1  << (channel_index * 4);            /* Set CCxE = 1, channel x output active. */
            }
            channel_index++;
        }
    }
    return OK;
}


t_error_handling tim_start(TIM_TypeDef *timer)
{
    uint8_t timer_index;
    for(timer_index = 0; timer_index < 3; timer_index++)
    {
        if(tim_driver[timer_index].timer == timer){break;}
    }
    
    //timer->CCER |= 0x1 << (channel * 4);
    timer->CR1 |= 1;
    return OK;
}


t_error_handling tim_stop(TIM_TypeDef *timer)
{
    uint8_t timer_index;
    for(timer_index = 0; timer_index < 3; timer_index++)
    {
        if(tim_driver[timer_index].timer == timer){break;}
    }
    timer->CR1 &= ~1;
    return OK;
}


t_error_handling tim_wait(TIM_TypeDef *timer, uint8_t channel)
{
    timer->SR &= (~0x1) << channel;  /* Clear event flags. */
    while((timer->SR & (0x1 << channel)) == 0){}

    return OK;
}


t_error_handling tim_wait_input_capture(TIM_TypeDef *timer, uint8_t channel)
{
	uint16_t local_return_value;
    timer->SR &= (~0x11) << channel;  /* Clear event flags. */
    while((timer->SR & (0x1 << channel)) == 0){}

    if(channel == 1)
    {
        local_return_value = timer->CCR1;
    }
    else if(channel == 2)
    {
        local_return_value = timer->CCR2;
    }
    else if(channel == 3)
    {
        local_return_value = timer->CCR3;
    }
    else if(channel == 4)
    {
        local_return_value = timer->CCR4;
    }
    timer->SR &= (~0x11) << channel;  /* Clear event flags. */
    return local_return_value;
}

extern void TIM1_IRQHandler(void)
{
    /**	TIM1 IRQ handler.
    *
    * \param void : No parameter.
    * \return : No return value.
    */

    clear_pending_nvic_irq(IRQ_TIM1_CC);
    if((TIM1->SR & 0x2) == 0x2)
    {
        TIM1->SR &= ~0x22;
        tim1_callback[0](TIM1->CCR1);
    }
    else if((TIM1->SR & 0x4) == 0x4)
    {
        TIM1->SR &= ~0x44;
        tim1_callback[1](TIM1->CCR2);
    }
    else if((TIM1->SR & 0x8) == 0x8)
    {
        TIM1->SR &= ~0x88;
        tim1_callback[2](TIM1->CCR3);
    }
    else if((TIM1->SR & 0x10) == 0x10)
    {
        TIM1->SR &= ~0x1010;
        tim1_callback[3](TIM1->CCR4);
    }
}
