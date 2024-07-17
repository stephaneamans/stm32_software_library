/*
 * clock.c
 *
 * Created on: Apr 20, 2019
 * Author: Stephane Amans
 */

/* Include files.        */
#include <string.h>

#include "fault.h"
#include "clock.h"

#include "regbase_flash.h"
#include "regbase_reset_clock.h"

#include "soc_configuration.h"

/* Bits masks */
#define RCC_CR_HSION_BIT_MASK             0x1
#define RCC_CR_HSIRDY_BIT_MASK            0x2
#define RCC_CR_HSEON_BIT_MASK         0x10000
#define RCC_CR_HSERDY_BIT_MASK        0x20000
#define RCC_CR_HSEBYP_BIT_MASK        0x40000
#define RCC_CR_CSSON_BIT_MASK         0x80000
#define RCC_CR_PLLON_BIT_MASK       0x1000000
#define RCC_CR_PLLRDY_BIT_MASK      0x2000000

#define RCC_CFGR_HSE_BIT_MASK             0x1
#define RCC_CFGR_PLL_FIELD_MASK           0x2
#define RCC_CFGR_SW_FIELD_MASK            0x3
#define RCC_CFGR_SWS_FIELD_MASK           0xC
#define RCC_CFGR_HPRE_FIELD_MASK         0xF0
#define RCC_CFGR_PPRE1_FIELD_MASK       0x700
#define RCC_CFGR_PPRE2_FIELD_MASK      0x3800
#define RCC_CFGR_PLL_SRC_BIT_MASK     0x10000
#define RCC_CFGR_PLLXTPRE_BIT_MASK    0x20000
#define RCC_CFGR_SYCLK_OUT_BIT_MASK 0x4000000
#define RCC_CFGR_HSI_OUT_BIT_MASK   0x5000000
#define RCC_CFGR_HSE_OUT_BIT_MASK   0x6000000
#define RCC_CFGR_PLL_OUT_BIT_MASK   0x7000000

#define FLASH_ACR_LATENCY_FIELD_MASK      0x7
#define FLASH_ACR_PRFTBE_BIT_MASK        0x10
#define FLASH_ACR_PRFTBS_BIT_MASK        0x20

#define CLK_ENABLE_DMA1_BIT_MASK         0x01
#define CLK_ENABLE_AFIO_BIT_MASK         0x01
#define CLK_ENABLE_PORTA_BIT_MASK        0x04
#define CLK_ENABLE_PORTB_BIT_MASK        0x08
#define CLK_ENABLE_PORTC_BIT_MASK        0x10
#define CLK_ENABLE_PORTD_BIT_MASK        0x20
#define CLK_ENABLE_SPI1_BIT_MASK       0x1000
#define CLK_ENABLE_SPI2_BIT_MASK       0x4000
#define CLK_ENABLE_TIM1_BIT_MASK        0x800
#define CLK_ENABLE_TIM2_BIT_MASK         0x01
#define CLK_ENABLE_TIM3_BIT_MASK         0x02
#define CLK_ENABLE_USART1_BIT_MASK     0x4000
#define CLK_ENABLE_USART2_BIT_MASK    0x20000


/* Static driver structure. */
struct t_clock_driver clock_driver;

/** Initialize and activate the PLL.
 *
 * \param sys_clk: System clock frequency.
 * \param source: Clock source to drive oscillator.
 *
 * \return: Error code or ERROR_OK.
 *
 */
static t_error_handling init_activate_PLL(uint32_t sys_clk, enum t_clock_sources source)
{
    uint8_t PLL_mult[15] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint32_t temp_Pll_mul = 0;                     /* Local variable: compute the PLL multiplier coefficient.     */
    uint32_t temp_clk_freq_MHZ = CLOCK_FREQ_MHZ;
    uint8_t error = ERROR_WRONG_CLOCK_SET;

    /* Disable PLL and wait for PLL is disabled. */
    RCC->CR &= ~RCC_CR_PLLON_BIT_MASK;
    while((RCC->CR & RCC_CR_PLLRDY_BIT_MASK) != 0){}

    /* Clear the PLL multiplier, the PLL HSE divider and the PLL source fields.  */
    RCC->CFGR &= 0xFFC0FFFF;

    if((((sys_clk % CLOCK_FREQ_MHZ) != 0)          /* Branch if sysclk is not a multiple of the ext crystal,        */
      || (sys_clk == CLOCK_FREQ_MHZ)) &&           /* Or if sysclk is equal to the ext crystal,                     */
         (source == PLL_HSE))                      /* And the clock is from HSE (bypassed or not) oscillator,       */
    {
        RCC->CFGR |= RCC_CFGR_PLLXTPRE_BIT_MASK;   /* PLLXTPRE = 1, divide the HSE input oscillator by 2.           */
        temp_clk_freq_MHZ /= 2;
    }

    if((source == HSI_OSC) || (source == PLL_HSI)){temp_clk_freq_MHZ /= 2;}

    temp_Pll_mul = sys_clk / temp_clk_freq_MHZ;    /* Compute the coefficient to load into the PLL multiplier.      */
    for(uint8_t index = 0; index <= 14; index++)   /* Screen the PLL multiplier table.                              */
    {
        if((temp_clk_freq_MHZ * PLL_mult[index]) == sys_clk)
        {
            temp_Pll_mul = PLL_mult[index] - 2;
            temp_Pll_mul = (temp_Pll_mul & 0x000000FF) << 18;

            RCC->CFGR |= temp_Pll_mul;

            /* If HSE oscillator is selected, select the HSE PLL source.     */
            if((source == PLL_HSE) || (source == PLL_HSE_BYP))
            {
                RCC->CFGR |= RCC_CFGR_PLL_SRC_BIT_MASK;
            }
            /* Enable PLL and wait for PLL ready. */
            RCC->CR |= RCC_CR_PLLON_BIT_MASK;
            while((RCC->CR & RCC_CR_PLLRDY_BIT_MASK) != RCC_CR_PLLRDY_BIT_MASK){}
            error = ERROR_OK;
            break;
        }
    }
    return error;
}

/** Initialize AHB clock.
 *
 * \param sys_clk: System clock frequency.
 * \param AHB_clk: AHB clock frequency.
 *
 * \return: Error code or ERROR_OK.
 *
 */
static t_error_handling init_AHB(uint32_t sys_clk, uint32_t AHB_clk)
{
    uint16_t AHB_prescaler[9] = {1, 2, 4, 8, 16, 64, 128, 256, 512};

    /* Compute the coefficient to load into the PLL multiplier. */
    uint32_t local_AHB_div = (sys_clk / AHB_clk);
    uint8_t error = ERROR_WRONG_CLOCK_SET;

    /* Screen the AHB prescaler table. */
    for(uint8_t index = 0; index < 9; index++)
    {
        if((sys_clk / AHB_prescaler[index]) == AHB_clk)
        {
            if(index == 0)
            {
            	local_AHB_div = 0;
            }
            else
            {
            	local_AHB_div = index + 7;
            }
            local_AHB_div = (local_AHB_div & 0x000000FF) << 4;
            RCC->CFGR &= ~RCC_CFGR_HPRE_FIELD_MASK;
            RCC->CFGR |= local_AHB_div;
            error = ERROR_OK;
            break; /* Exit the loop, the value has been found. */
        }
    }
    return error;
}

/** Initialize APB1 clock.
 *
 * \param AHB_clk: AHB clock frequency.
 * \param APB1_clk: APB1 clock frequency.
 *
 * \return: Error code or ERROR_OK.
 *
 */
static t_error_handling init_APB1(uint32_t AHB_clk, uint32_t APB1_clk)
{
    uint8_t APB_prescaler[5] = {1, 2, 4, 8, 16};

    /* Compute the coefficient to load into the PLL multiplier. */
    uint32_t local_APB1_div = (AHB_clk / APB1_clk);
    uint8_t error = ERROR_WRONG_CLOCK_SET;

    /* Screen the APB1 prescaler table. */
    for(uint8_t index = 0; index < 5; index++)
    {
        if((AHB_clk / APB_prescaler[index]) == APB1_clk)
        {
            if(index == 0)
            {
            	local_APB1_div = 0;
            }
            else
            {
            	local_APB1_div = index + 3;
            }
            local_APB1_div = (local_APB1_div & 0x000000FF) << 8;
            RCC->CFGR &= ~RCC_CFGR_PPRE1_FIELD_MASK;
            RCC->CFGR |= local_APB1_div;
            error = ERROR_OK;
            break; /* Exit the loop, the value has been found. */
        }
    }
    return error;
}

/** Initialize APB2 clock.
 *
 * \param AHB_clk: AHB clock frequency.
 * \param APB2_clk: APB2 clock frequency.
 *
 * \return: Error code or ERROR_OK.
 *
 */
static t_error_handling init_APB2(uint32_t AHB_clk, uint32_t APB2_clk)
{
    uint8_t APB_prescaler[5] = {1, 2, 4, 8, 16};

    /* Compute the coefficient to load into the PLL multiplier. */
    uint32_t local_APB2_div = (AHB_clk / APB2_clk);
    uint8_t error = ERROR_WRONG_CLOCK_SET;

    /* Screen the APB2 prescaler table. */
    for(uint8_t index = 0; index < 5; index++)
    {
        if((AHB_clk / APB_prescaler[index]) == APB2_clk)
        {
            if(index == 0)
            {
            	local_APB2_div = 0;
            }
            else
            {
            	local_APB2_div = index + 3;
            }
            local_APB2_div = (local_APB2_div & 0x000000FF) << 11;
            RCC->CFGR &= ~RCC_CFGR_PPRE2_FIELD_MASK;
            RCC->CFGR |= local_APB2_div;
            error = ERROR_OK;
            break; /* Exit the loop, the value has been found. */
        }
    }
    return error;
}

/** Start oscillator function of the clock source.
 *
 * \param source: Clock source to drive oscillator.
 *
 */
static void start_oscillator(enum t_clock_sources source)
{
    if((source == HSI_OSC) || (source == PLL_HSI))
    {
        RCC->CR |= RCC_CR_HSION_BIT_MASK;
        while((RCC->CR & RCC_CR_HSIRDY_BIT_MASK) != RCC_CR_HSIRDY_BIT_MASK){}
    }
    else if((source == HSE_OSC) || (source == PLL_HSE))
    {
        RCC->CR |= RCC_CR_HSEON_BIT_MASK;
        while((RCC->CR & RCC_CR_HSERDY_BIT_MASK) != RCC_CR_HSERDY_BIT_MASK){}
    }
    else if((source == HSE_BYP) || (source == PLL_HSE_BYP))
    {
        RCC->CR |= (RCC_CR_HSEBYP_BIT_MASK | RCC_CR_HSEON_BIT_MASK);
        while((RCC->CR & RCC_CR_HSIRDY_BIT_MASK) != RCC_CR_HSIRDY_BIT_MASK){}
    }
}

/** Set flash latency.
 *
 * \param sys_clk: System clock frequency.
 *
 */
static void set_flash_latency(uint32_t sys_clk)
{
    uint32_t temp_flash_ACR = 0;
    start_oscillator(HSI_OSC);

    /* If the system clock is between 0 and 24 MHz, */
    /* LATENCY = 0, 0 wait states configured.       */
    if((sys_clk > 0) && (sys_clk <= 24000000))
    {
        FLASH->ACR &= ~FLASH_ACR_LATENCY_FIELD_MASK;
    }

    /* If the system clock is between 24 and 48 MHz, */
    /* Store the FLASH_ACR register.                 */
    /* LATENCY	= 1, 1 wait state is configured.     */
    else if((sys_clk > 24000000) && (sys_clk <= 48000000))
    {
        temp_flash_ACR = FLASH->ACR;
        FLASH->ACR = ((temp_flash_ACR & ~FLASH_ACR_LATENCY_FIELD_MASK)+1);
    }

    /* If the system clock is between 48 and 72 MHz, */
    /* Store the FLASH_ACR register.                 */
    /* LATENCY = 2, 2 wait states are configured.    */
    else if((sys_clk > 48000000) && (sys_clk <= 72000000))
    {
        temp_flash_ACR = FLASH->ACR;
        FLASH->ACR = ((temp_flash_ACR & ~FLASH_ACR_LATENCY_FIELD_MASK)+2);
    }
    FLASH->ACR |= 0x00000010;                        /* LATENCY = 0, 0 wait states configured. */
    while((FLASH->ACR & FLASH_ACR_PRFTBS_BIT_MASK) != FLASH_ACR_PRFTBS_BIT_MASK){} /* Wait for PLLRDY = 1, PLL is stable.    */
}

/** Stop oscillator.
 *
 * \param source: Clock source to drive oscillator..
 *
 */
static void stop_oscillator(enum t_clock_sources source)
{
    if(source == HSI_OSC)
    {
        RCC->CR &= ~RCC_CR_HSEON_BIT_MASK; /* Clear HSION bit. */
    }
    else if(source == HSE_OSC)
    {
        RCC->CR &= ~RCC_CR_HSEON_BIT_MASK; /* Clear HSEON bit. */
    }
    else if(source == HSE_BYP)
    {
        RCC->CR &= ~(RCC_CR_HSEON_BIT_MASK | RCC_CR_HSEBYP_BIT_MASK); /* Clear HSEON and HSEBYP bits. */
    }
}

/** Switch the system clock source.
 *
 * \param source: Clock source to drive oscillator.
 *
 */
static void switch_system_clk(enum t_clock_sources source)
{
    uint8_t local_source = (RCC->CFGR & RCC_CFGR_SWS_FIELD_MASK) >> 2;

    if((local_source != source) && (source == HSI_OSC))
    {
        RCC->CFGR &= ~RCC_CFGR_SW_FIELD_MASK; /* SW = 0, Select the HSI oscillator. */
    }
    else if((local_source != source) && (source == HSE_OSC))
    {
        /* If local source is HSI oscillator.          */
        /* SW = 1, Disable HSI oscillator, LSB is set. */
        if(local_source == 0x00)
        {
            RCC->CFGR |= RCC_CFGR_HSE_BIT_MASK;
        }
        /* If local source is PLL.                 */
        /* SW = 1, Disable PLL, invert the 2 bits. */
        else if(local_source == 0x02)
        {
            RCC->CFGR ^= RCC_CFGR_SW_FIELD_MASK;
        }
    }
    else if((local_source != source) && ((source >= PLL_HSI) && (source <= PLL_HSE_BYP)))
    {
    	/* If local source is HSI oscillator.           */
    	/* SW = 2, Disable HSI oscillator , MSB is set. */
        if(local_source == 0x00)
        {
            RCC->CFGR |= RCC_CFGR_PLL_FIELD_MASK;
        }
        /* If local source is HSE oscillator.      */
        /* SW = 2, Disable PLL, invert the 2 bits. */
        else if(local_source == 0x01)
        {
            RCC->CFGR ^= RCC_CFGR_SW_FIELD_MASK;
        }
    }
}

void disable_clock(enum t_peripheral peripheral)
{
	switch(peripheral)
	{
	case DMA_1:
		RCC->AHBENR &= ~CLK_ENABLE_DMA1_BIT_MASK;
		break;

	case PORT_A:
        RCC->APB2ENR &= ~CLK_ENABLE_PORTA_BIT_MASK;
        break;

	case PORT_B:
		RCC->APB2ENR &= ~CLK_ENABLE_PORTB_BIT_MASK;
	    break;

	case PORT_C:
        RCC->APB2ENR &= ~CLK_ENABLE_PORTC_BIT_MASK;
	    break;

	case PORT_D:
        RCC->APB2ENR &= ~CLK_ENABLE_PORTD_BIT_MASK;
        break;

    case PORT_E:
        break;

    case PORT_F:
        break;

    case PORT_G:
        break;

	case SPI1:
        RCC->APB2ENR &= ~CLK_ENABLE_SPI1_BIT_MASK;
	    break;

    case SPI2:
        RCC->APB1ENR &= ~CLK_ENABLE_SPI2_BIT_MASK;
        break;

	case TIM_1:
        RCC->APB2ENR &= ~CLK_ENABLE_TIM1_BIT_MASK;
        break;

    case TIM_2:
        RCC->APB1ENR &= ~CLK_ENABLE_TIM2_BIT_MASK;
        break;

    case TIM_3:
        RCC->APB1ENR &= ~CLK_ENABLE_TIM3_BIT_MASK;
        break;

    case USART1:
        RCC->APB2ENR &= ~CLK_ENABLE_USART1_BIT_MASK;
        break;

    case USART2:
        RCC->APB1ENR &= ~CLK_ENABLE_USART2_BIT_MASK;
        break;
	}
}

t_error_handling enable_clock(enum t_peripheral peripheral)
{
	t_error_handling error = ERROR_OK;
    switch(peripheral)
    {
    case DMA_1:
    	RCC->AHBENR |= CLK_ENABLE_DMA1_BIT_MASK;
    	if((RCC->AHBENR & CLK_ENABLE_DMA1_BIT_MASK) != CLK_ENABLE_DMA1_BIT_MASK)
    	{
    		error = ERROR_WRITTEN_VALUE_CORRUPTED;
    	}
    	break;

    case PORT_A:
    	RCC->APB2ENR |= CLK_ENABLE_PORTA_BIT_MASK;
        RCC->APB2ENR |= CLK_ENABLE_AFIO_BIT_MASK;
    	if(((RCC->APB2ENR & CLK_ENABLE_PORTA_BIT_MASK) != CLK_ENABLE_PORTA_BIT_MASK) ||
           ((RCC->APB2ENR & CLK_ENABLE_AFIO_BIT_MASK) != CLK_ENABLE_AFIO_BIT_MASK))
    	{
    	    error = ERROR_WRITTEN_VALUE_CORRUPTED;
    	}
        break;

    case PORT_B:
        RCC->APB2ENR |= CLK_ENABLE_PORTB_BIT_MASK;
        RCC->APB2ENR |= CLK_ENABLE_AFIO_BIT_MASK;
        if(((RCC->APB2ENR & CLK_ENABLE_PORTB_BIT_MASK) != CLK_ENABLE_PORTB_BIT_MASK) ||
           ((RCC->APB2ENR & CLK_ENABLE_AFIO_BIT_MASK) != CLK_ENABLE_AFIO_BIT_MASK))
    
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;

    case PORT_C:
        RCC->APB2ENR |= CLK_ENABLE_PORTC_BIT_MASK;
        RCC->APB2ENR |= CLK_ENABLE_AFIO_BIT_MASK;
        if(((RCC->APB2ENR & CLK_ENABLE_PORTC_BIT_MASK) != CLK_ENABLE_PORTC_BIT_MASK) ||
           ((RCC->APB2ENR & CLK_ENABLE_AFIO_BIT_MASK) != CLK_ENABLE_AFIO_BIT_MASK))
    
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;

    case PORT_D:
        RCC->APB2ENR |= CLK_ENABLE_PORTD_BIT_MASK;
        RCC->APB2ENR |= CLK_ENABLE_AFIO_BIT_MASK;
        if(((RCC->APB2ENR & CLK_ENABLE_PORTD_BIT_MASK) != CLK_ENABLE_PORTD_BIT_MASK) ||
           ((RCC->APB2ENR & CLK_ENABLE_AFIO_BIT_MASK) != CLK_ENABLE_AFIO_BIT_MASK))
    
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;

    case PORT_E:
        break;

    case PORT_F:
        break;

    case PORT_G:
        break;

    case SPI1:
        RCC->APB2ENR |= CLK_ENABLE_SPI1_BIT_MASK;
        if((RCC->APB2ENR & CLK_ENABLE_SPI1_BIT_MASK) != CLK_ENABLE_SPI1_BIT_MASK)
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;

    case SPI2:
        RCC->APB1ENR |= CLK_ENABLE_SPI2_BIT_MASK;
        if((RCC->APB1ENR & CLK_ENABLE_SPI2_BIT_MASK) != CLK_ENABLE_SPI2_BIT_MASK)
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;

    case TIM_1:
        RCC->APB2ENR |= CLK_ENABLE_TIM1_BIT_MASK;
        if((RCC->APB2ENR & CLK_ENABLE_TIM1_BIT_MASK) != CLK_ENABLE_TIM1_BIT_MASK)
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;

    case TIM_2:
        RCC->APB1ENR |= CLK_ENABLE_TIM2_BIT_MASK;
        if((RCC->APB1ENR & CLK_ENABLE_TIM2_BIT_MASK) != CLK_ENABLE_TIM2_BIT_MASK)
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;

    case TIM_3:
        RCC->APB1ENR |= CLK_ENABLE_TIM3_BIT_MASK;
        if((RCC->APB1ENR & CLK_ENABLE_TIM3_BIT_MASK) != CLK_ENABLE_TIM3_BIT_MASK)
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;

    case USART1:
        RCC->APB2ENR |= CLK_ENABLE_USART1_BIT_MASK;
        if((RCC->APB2ENR & CLK_ENABLE_USART1_BIT_MASK) != CLK_ENABLE_USART1_BIT_MASK)
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;

    case USART2:
        RCC->APB1ENR |= CLK_ENABLE_USART2_BIT_MASK;
        if((RCC->APB1ENR & CLK_ENABLE_USART2_BIT_MASK) != CLK_ENABLE_USART2_BIT_MASK)
        {
            error = ERROR_WRITTEN_VALUE_CORRUPTED;
        }
        break;
    }
    return error;
}

void reset_module(enum t_peripheral peripheral)
{
    switch(peripheral)
    {
    case DMA_1:
        break;

    case PORT_A:
        RCC->APB2RSTR |= CLK_ENABLE_PORTA_BIT_MASK | CLK_ENABLE_AFIO_BIT_MASK;
        break;

    case PORT_B:
        RCC->APB2RSTR |= CLK_ENABLE_PORTB_BIT_MASK | CLK_ENABLE_AFIO_BIT_MASK;
        break;

    case PORT_C:
        RCC->APB2RSTR |= CLK_ENABLE_PORTC_BIT_MASK | CLK_ENABLE_AFIO_BIT_MASK;
        break;

    case PORT_D:
        RCC->APB2RSTR |= CLK_ENABLE_PORTD_BIT_MASK | CLK_ENABLE_AFIO_BIT_MASK;
        break;

    case PORT_E:
        break;

    case PORT_F:
        break;

    case PORT_G:
        break;

    case SPI1:
        RCC->APB2RSTR |= CLK_ENABLE_SPI1_BIT_MASK;
        break;

    case SPI2:
        RCC->APB1RSTR |= CLK_ENABLE_SPI2_BIT_MASK;
        break;

    case TIM_1:
        RCC->APB2RSTR |= CLK_ENABLE_TIM1_BIT_MASK;
        break;

    case TIM_2:
        RCC->APB1RSTR |= CLK_ENABLE_TIM2_BIT_MASK;
        break;

    case TIM_3:
        RCC->APB1RSTR |= CLK_ENABLE_TIM3_BIT_MASK;
        break;

    case USART1:
        RCC->APB2RSTR |= CLK_ENABLE_USART1_BIT_MASK;
        break;

    case USART2:
        RCC->APB1RSTR |= CLK_ENABLE_USART2_BIT_MASK;
        break;
    }
}

struct t_clock_driver *get_clock_driver(void)
{
    return &clock_driver;
}

void output_clock(enum t_clock_sources source)
{
    uint32_t local_MCO_mask = 0x07000000;
    RCC->CFGR &= ~local_MCO_mask;
    if(source == SYSCLOCK)
    {
    	RCC->CFGR |= RCC_CFGR_SYCLK_OUT_BIT_MASK;
    }
    else if(source == HSI_OSC)
    {
    	RCC->CFGR |= RCC_CFGR_HSI_OUT_BIT_MASK;
    }
    else if(source == HSE_OSC)
    {
    	RCC->CFGR |= RCC_CFGR_HSE_OUT_BIT_MASK;
    }
    else if((source >= PLL_HSI) &&
            (source <= PLL_HSE_BYP))
    {
    	RCC->CFGR |= RCC_CFGR_PLL_OUT_BIT_MASK;
    }
    else
    {
    	RCC->CFGR &= ~local_MCO_mask;
    }
}

t_error_handling clock_init(struct t_clock_driver *driver, struct t_clock_config *config)
{
    uint8_t error = ERROR_OK;

    /* Clear the driver */
    memset(driver, 0, sizeof(struct t_clock_driver));

    /* Fill the driver structure fields */
    driver->sys_clk_freq = config->sys_clk_freq;
    driver->AHB_clk_freq = config->AHB_clk_freq;
    driver->APB1_clk_freq = config->APB1_clk_freq;
    driver->APB2_clk_freq = config->APB2_clk_freq;
    driver->source = config->source;

    start_oscillator(driver->source);

    /* PLL needs to be activated if system clock is greater than osc. */
    if ((driver->source >= PLL_HSI) && (driver->source <= PLL_HSE_BYP))
    {
        start_oscillator(HSI_OSC);
        switch_system_clk(HSI_OSC);
        error = init_activate_PLL(driver->sys_clk_freq, driver->source); /* Set PLL. */
        if(error != ERROR_OK)
        {
        	goto end;
        }
    }

    /* Flash latency depends on system clock frequency. */
    set_flash_latency(driver->sys_clk_freq);

    switch_system_clk(driver->source);

    /* If High Speed Internal Oscillator (HSI) is selected,         */
    /* Stop High Speed External Oscillator (HSE) and the bypass.    */
    if((driver->source == HSI_OSC) || (driver->source == PLL_HSI))
    {
        stop_oscillator(HSE_BYP);
    }
    else if ((driver->source == HSE_OSC) || (driver->source == HSE_BYP)
                                         || (driver->source == PLL_HSE)
                                         || (driver->source == PLL_HSE_BYP))
    {
        stop_oscillator(HSI_OSC);

        /* If High Speed External Oscillator (HSE) is selected,         */
        /* And deactivate the bypass.                                   */
        if((driver->source == HSE_OSC) || (driver->source == PLL_HSE))
        {
            RCC->CR &= ~(RCC_CR_HSEON_BIT_MASK | RCC_CR_HSERDY_BIT_MASK | RCC_CR_CSSON_BIT_MASK);
        }
    }
    error = init_AHB(driver->sys_clk_freq, driver->AHB_clk_freq);   /* Set AHB prescaler.     */
    if(error != ERROR_OK)
    {
        goto end;
    }

    error = init_APB1(driver->AHB_clk_freq, driver->APB1_clk_freq); /* Set APB1 prescaler.    */
    if(error != ERROR_OK)
    {
        goto end;
    }

    error = init_APB2(driver->AHB_clk_freq, driver->APB2_clk_freq); /* Set APB2 prescaler.    */
    if(error != ERROR_OK)
    {
        goto end;
    }

end:
    if(error != ERROR_OK)
    {
    	fault_log_error(error);
    }
    return error;
}

