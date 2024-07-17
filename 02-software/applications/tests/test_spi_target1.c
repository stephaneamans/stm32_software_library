/*
 * test_spi.c
 *
 * Created on: Jul 4, 2021
 * Author: Stephane Amans
 */

/* Include files.        */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "clock.h"
#include "dma.h"

#include "test.h"
#include "spi.h"

/* Defines */
#define BUFFER_LENGTH_256   256

struct t_spi_slave component[3] =
{
    [0] =
    {
        .freq_khz = 4000,
        .data_length = spi_data_8_bits,
        .frame_direction = spi_frame_msb_first,
        .clock_phase = spi_clk_first,
        .clock_polarity = spi_clk_rising,
    },

    [1] =
    {
        .freq_khz = 1125,
        .data_length = spi_data_8_bits,
        .frame_direction = spi_frame_msb_first,
        .clock_phase = spi_clk_first,
        .clock_polarity = spi_clk_rising,
    },

    [2] =
    {
        .freq_khz = 18000,
        .data_length = spi_data_8_bits,
        .frame_direction = spi_frame_msb_first,
        .clock_phase = spi_clk_first,
        .clock_polarity = spi_clk_rising,
    },
};

t_error_handling spi_test(void)
{

    component[0].cs = &pa8;
    component[1].cs = &pa8;
    component[2].cs = &pa8;

    /* Declarations. */
    uint16_t buffer_write[BUFFER_LENGTH_256];
    uint16_t buffer_read[BUFFER_LENGTH_256];
    t_error_handling error = ERROR_OK;
    struct t_spi_data data;

    for (uint16_t i = 0; i < BUFFER_LENGTH_256; i++)
    {
        buffer_write[i] = 0;
        buffer_read[i] = 0;
    }

    buffer_write[0] = 0x02;
    buffer_write[1] = 0xFF;
    buffer_write[2] = 0xFF;
    data.write_buffer = &buffer_write[0];
    data.read_buffer = &buffer_read[0];
    data.length = 3;



    //struct t_spi_driver *spi1 = &spi_driver[0];

   	spi_slave_register(&component[0]);
   	spi_slave_register(&component[1]);
   	spi_slave_register(&component[2]);

   	gpio_write(component[1].cs, true);
    error = spi_transfer(&spi1, &component[1], &data);
   
    while(spi_transfer_status(&spi1, &component[1]) != ERROR_OK){}

    while(1)
    {

    }

    return error;
}
