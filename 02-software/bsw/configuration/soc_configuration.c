#include "soc_configuration.h"

struct t_clock_config clock_config =
{
    .sys_clk_freq = 72 MHZ,
    .AHB_clk_freq = 36 MHZ,
    .APB1_clk_freq = 36 MHZ,
    .APB2_clk_freq = 36 MHZ,
    .source = PLL_HSE,
};

struct t_systick_driver systick =
{
    .systick_on_processor_clock = true,
    .reload_value = 0xFFFFFF,
    .irq.active = false,
    .irq.callback = 0,
};

struct t_dma_driver dma1 =
{
    .base_address_dma                   = (AHBPERIPH_BASE + 0x00000000U),
    .peripheral                         = DMA_1,
    .instance                           = 0
};

struct t_dma_channel_driver dma_channel1 = 
{
    .base_address_dma_channel        = (AHBPERIPH_BASE + 0x00000008U),
    .channel_number                  = 0,
    .mem2mem                         = true,
    .dma_priority_level              = 0,
    .irq.priority                    = 0,
    .irq.transfer_complete           = false,
    .irq.half_transfer_complete      = false,
    .irq.transfer_error              = false,
    .irq.callback                    = false,
};

struct t_dma_channel_driver dma_channel2 = 
{
    .base_address_dma_channel        = (AHBPERIPH_BASE + 0x0000001CU),
    .channel_number                  = 1,
    .mem2mem                         = false,
    .dma_priority_level              = 0,
    .irq.priority                    = 0,
    .irq.transfer_complete           = true,
    .irq.half_transfer_complete      = false,
    .irq.transfer_error              = false,
    .irq.callback                    = &spi_dma_ch2_irq_management,
};

struct t_dma_channel_driver dma_channel3 = 
{
    .base_address_dma_channel        = (AHBPERIPH_BASE + 0x00000030U),
    .channel_number                  = 2,
    .mem2mem                         = false,
    .dma_priority_level              = 0,
    .irq.priority                    = 0,
    .irq.transfer_complete           = true,
    .irq.half_transfer_complete      = false,
    .irq.transfer_error              = false,
    .irq.callback                    = 0,
};

struct t_dma_channel_driver dma_channel4 = 
{
    .base_address_dma_channel        = (AHBPERIPH_BASE + 0x00000044U),
    .channel_number                  = 3,
    .mem2mem                         = false,
    .dma_priority_level              = 0,
    .irq.priority                    = 0,
    .irq.transfer_complete           = true,
    .irq.half_transfer_complete      = false,
    .irq.transfer_error              = false,
    .irq.callback                    = &usart1_dma_ch4_irq_management,
};

struct t_dma_channel_driver dma_channel5 = 
{
    .base_address_dma_channel        = (AHBPERIPH_BASE + 0x00000058U),
    .channel_number                  = 4,
    .mem2mem                         = false,
    .dma_priority_level              = 0,
    .irq.priority                    = 0,
    .irq.transfer_complete           = true,
    .irq.half_transfer_complete      = false,
    .irq.transfer_error              = false,
    .irq.callback                    = &usart1_dma_ch5_irq_management,
};

struct t_dma_channel_driver dma_channel6 = 
{
    .base_address_dma_channel        = (AHBPERIPH_BASE + 0x0000006CU),
    .channel_number                  = 5,
    .mem2mem                         = false,
    .dma_priority_level              = 0,
    .irq.priority                    = 0,
    .irq.transfer_complete           = true,
    .irq.half_transfer_complete      = false,
    .irq.transfer_error              = false,
    .irq.callback                    = 0,
};

struct t_dma_channel_driver dma_channel7 = 
{
    .base_address_dma_channel        = (AHBPERIPH_BASE + 0x00000080U),
    .channel_number                  = 6,
    .mem2mem                         = false,
    .dma_priority_level              = 0,
    .irq.priority                    = 0,
    .irq.transfer_complete           = false,
    .irq.half_transfer_complete      = false,
    .irq.transfer_error              = false,
    .irq.callback                    = false,
};

struct t_exti_config exti_config =
{
    .base_address_exti = (APB2PERIPH_BASE + 0x00000400U),
};

struct t_afio_config afio_config =
{
    .base_address_afio = (APB2PERIPH_BASE + 0x00000000U),
};

struct t_gpio_driver pa0 =
{
    .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
    .exti_config = &exti_config,
    .afio_config = &afio_config,
    .pin = 0,
    .type = input_pullup,
    .peripheral = PORT_A,
    .instance = 0,
    .irq.active = false,
    .irq.rising = false,
    .irq.priority = prio0,
    .irq.callback = 0,
};

struct t_gpio_driver pa1 =
{
    .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
    .exti_config = &exti_config,
    .afio_config = &afio_config,
    .pin = 1,
    .type = input_floating,
    .peripheral = PORT_A,
    .instance = 1,
    .irq.active = false,
    .irq.rising = false,
    .irq.priority = prio0,
    .irq.callback = 0,
};

struct t_gpio_driver pa2 =
{
    .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
    .exti_config = &exti_config,
    .afio_config = &afio_config,
    .pin = 2,
    .type = output_pushpull_50MHz,
    .peripheral = PORT_A,
    .instance = 2,
    .irq.active = false,
    .irq.rising = false,
    .irq.priority = prio0,
    .irq.callback = 0,
};

struct t_gpio_driver pa3 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 3,
  .type = input_pullup,
  .peripheral = PORT_A,
  .instance = 3,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa4 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 4,
  .type = output_pushpull_10MHz,
  .peripheral = PORT_A,
  .instance = 4,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa5 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 5,
  .type = output_altpushpull_2MHz,
  .peripheral = PORT_A,
  .instance = 5,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa6 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 6,
  .type = input_floating,
  .peripheral = PORT_A,
  .instance = 6,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa7 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 7,
  .type = output_altpushpull_2MHz,
  .peripheral = PORT_A,
  .instance = 7,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa8 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 8,
  .type = output_altpushpull_50MHz,
  .peripheral = PORT_A,
  .instance = 8,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa9 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 9,
  .type = output_altpushpull_2MHz,
  .peripheral = PORT_A,
  .instance = 9,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa10 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 10,
  .type = input_floating,
  .peripheral = PORT_A,
  .instance = 10,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa11 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 11,
  .type = output_altpushpull_10MHz,
  .peripheral = PORT_A,
  .instance = 11,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa12 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 12,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_A,
  .instance = 12,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa13 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 13,
  .type = output_opendrain_2MHz,
  .peripheral = PORT_A,
  .instance = 13,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa14 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 14,
  .type = input_pullup,
  .peripheral = PORT_A,
  .instance = 14,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pa15 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000800U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 15,
  .type = input_pullup,
  .peripheral = PORT_A,
  .instance = 15,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb0 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 0,
  .type = output_opendrain_10MHz,
  .peripheral = PORT_B,
  .instance = 16,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb1 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 1,
  .type = input_pullup,
  .peripheral = PORT_B,
  .instance = 17,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb2 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 2,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_B,
  .instance = 18,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb3 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 3,
  .type = input_pullup,
  .peripheral = PORT_B,
  .instance = 19,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb4 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 4,
  .type = input_pullup,
  .peripheral = PORT_B,
  .instance = 20,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb5 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 5,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_B,
  .instance = 21,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb6 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 6,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_B,
  .instance = 22,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb7 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 7,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_B,
  .instance = 23,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb8 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 8,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_B,
  .instance = 24,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb9 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 9,
  .type = input_pullup,
  .peripheral = PORT_B,
  .instance = 25,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb10 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 10,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_B,
  .instance = 26,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb11 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 11,
  .type = input_pullup,
  .peripheral = PORT_B,
  .instance = 27,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb12 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 12,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_B,
  .instance = 28,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
 };

struct t_gpio_driver pb13 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 13,
  .type = output_altpushpull_2MHz,
  .peripheral = PORT_B,
  .instance = 29,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb14 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 14,
  .type = input_pullup,
  .peripheral = PORT_B,
  .instance = 30,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pb15 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00000C00U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 15,
  .type = output_altpushpull_2MHz,
  .peripheral = PORT_B,
  .instance = 31,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pc13 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00001000U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 13,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_C,
  .instance = 32,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pc14 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00001000U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 14,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_C,
  .instance = 33,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pc15 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00001000U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 15,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_C,
  .instance = 34,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pd0 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00001400U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 0,
  .type = input_pullup,
  .peripheral = PORT_D,
  .instance = 35,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_gpio_driver pd1 =
{
  .base_address_gpio = (APB2PERIPH_BASE + 0x00001400U),
  .exti_config = &exti_config,
  .afio_config = &afio_config,
  .pin = 1,
  .type = output_pushpull_50MHz,
  .peripheral = PORT_D,
  .instance = 36,
  .irq.active = false,
  .irq.rising = false,
  .irq.priority = prio0,
  .irq.callback = 0,
};

struct t_timer_cfg timx_chanelx_cfg [TIMER_IP_NUMBER] =
{
	[0] =
	{
		.timer = TIM1,
		.time_frequency = 1 MSEC,
		.channel[0] =
		{
		    .channel_enable = enable,
		    .timer_mode = input_capture,
		    .tim_mux = ccx_channel_input_ICx_straight,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = pwm2,
		},
		.channel[1] =
		{
			.channel_enable = disable,
		    .timer_mode = 0,
		    .tim_mux = 0,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = 0,
		},
		.channel[2] =
		{
			.channel_enable = disable,
		    .timer_mode = 0,
		    .tim_mux = 0,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = 0,
		},
		.channel[3] =
		{
			.channel_enable = disable,
		    .timer_mode = 0,
		    .tim_mux = 0,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = 0,
		},
	},
	[1] =
	{
		.timer = TIM2,
	    .time_frequency = 1 MSEC,
	    .channel[0] =
	    {
    		.channel_enable = disable,
   		    .timer_mode = 0,
   		    .tim_mux = 0,
   		    .percent_pwm = 0,
  		    .polarity = 0,
   		    .irq_dma.active = 0,
   		    //.irq_dma.edge = 0,
   		    .irq_dma.priority = 0,
 //  		    .irq_dma.callback = 0,
   		    .out_cmp_mode = 0,
	    },
		.channel[1] =
		{
			.channel_enable = enable,
		    .timer_mode = input_capture,
		    .tim_mux = ccx_channel_input_ICx_straight,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = pwm2,
		},
		.channel[2] =
		{
			.channel_enable = disable,
		    .timer_mode = 0,
		    .tim_mux = 0,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = 0,
		},
		.channel[3] =
		{
			.channel_enable = disable,
		    .timer_mode = 0,
		    .tim_mux = 0,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = 0,
		},
	},
	[2] =
	{
		.timer = TIM3,
	    .time_frequency = 1 MSEC,
		.channel[0] =
		{
			.channel_enable = enable,
		    .timer_mode = output_compare,
		    .tim_mux = ccx_channel_output,
		    .percent_pwm = 25,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = pwm1,
		},
		.channel[1] =
		{
			.channel_enable = disable,
		    .timer_mode = 0,
		    .tim_mux = 0,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = 0,
		},
		.channel[2] =
		{
			.channel_enable = disable,
		    .timer_mode = 0,
		    .tim_mux = 0,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = 0,
		},
		.channel[3] =
		{
			.channel_enable = disable,
		    .timer_mode = 0,
		    .tim_mux = 0,
		    .percent_pwm = 0,
		    .polarity = 0,
		    .irq_dma.active = 0,
		    //.irq_dma.edge = 0,
		    .irq_dma.priority = 0,
//		    .irq_dma.callback = 0,
		    .out_cmp_mode = 0,
		},
	},
};

struct t_spi_driver spi1 =
{
    .base_address = (APB2PERIPH_BASE + 0x00003000U),
    .peripheral = SPI1,
    .instance = 0,
    .irq.active = false,
    .irq.priority = 0,
    .dma.active = false,
    .dma.tx_channel = &dma_channel3,
    .dma.rx_channel = &dma_channel2,
};

struct t_spi_driver spi2 =
{
    .base_address = (APB1PERIPH_BASE + 0x00003800U),
    .peripheral = SPI2,
    .instance = 1,
    .irq.active = false,
    .irq.priority = 0,
    .dma.active = false,
    .dma.tx_channel = &dma_channel5,
    .dma.rx_channel = &dma_channel4,
};

struct t_usart_driver usart1 =
{
    .base_address   = (APB2PERIPH_BASE + 0x00003800U),
    .peripheral     = USART1,
    .instance       = 0,
    .baudrate       = 115200,
    .data_length    = usart_data_9_bits,
    .parity         = usart_parity_odd,
    .stop_bit       = usart_one_stop_bit,
    .irq.active     = false,
    .irq.priority   = prio0,
    .dma.active     = false,
    .dma.tx_channel = &dma_channel2,
    .dma.rx_channel = &dma_channel3,
};

struct t_usart_driver usart2 =
{
    .base_address   = (APB1PERIPH_BASE + 0x00004400U),
    .peripheral     = USART2,
    .instance       = 1,
    .baudrate       = 115200,
    .data_length    = usart_data_8_bits,
    .parity         = usart_parity_odd,
    .stop_bit       = usart_one_stop_bit,
    .irq.active     = false,
    .irq.priority   = prio0,
    .dma.active     = false,
    .dma.tx_channel = 0,
    .dma.rx_channel = 0,
};
