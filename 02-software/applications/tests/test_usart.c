/*
 * test_usart.c
 *
 * Created on: Jun 12, 2020
 * Author: Stephane Amans
 */

/* Include files.        */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "clock.h"
#include "dma.h"

#include "test.h"
#include "usart.h"

/* Defines */
#define BUFFER_LENGTH_256   256

t_error_handling print_test_header(void)
{
    /* Declarations. */
    char buffer_tx_8bits[BUFFER_LENGTH_256];
    char buffer_rx_8bits[BUFFER_LENGTH_256];
    uint32_t string_length;
    t_error_handling error = ERROR_OK;

    for(uint16_t index_loop = 0; index_loop < BUFFER_LENGTH_256; index_loop++)
    {
    	buffer_tx_8bits[index_loop] = 0;
    	buffer_rx_8bits[index_loop] = 0;
    }


    string_length = sprintf(buffer_tx_8bits, "\r\nSTM32F test application\r\n-----------------------\n\n\rDo you want to start(yes/no): ");

    usart_transmit(&usart1, (uint8_t*)&buffer_tx_8bits, string_length);

    while(usart_status(&usart1, false) != ERROR_OK){}

    uint8_t index = 0;
    while(buffer_rx_8bits[index - 1] != 0x0d)
    {
    	usart_receive(&usart1, (uint8_t*)&buffer_rx_8bits[index], 1);
        while(usart_status(&usart1, true) != ERROR_OK){}
        index++;
    }

    if(strcmp(&buffer_rx_8bits[0], "yes\r") == 0)
    {
        string_length = sprintf(&buffer_tx_8bits[0], "\n\n\ryes, cool !");
        usart_transmit(&usart1, (uint8_t*)&buffer_tx_8bits, string_length);
        while(usart_status(&usart1, false) != ERROR_OK){}
    }
    else if(strcmp(&buffer_rx_8bits[0], "no\r") == 0)
    {
    	string_length = sprintf(&buffer_tx_8bits[0], "\n\n\roh non, pas cool !");
        usart_transmit(&usart1, (uint8_t*)&buffer_tx_8bits, string_length);
        while(usart_status(&usart1, false) != ERROR_OK){}
    }
    else
    {
     	string_length = sprintf(&buffer_tx_8bits[0], "\n\n\rbon, ben je ne sais pas ...");
        usart_transmit(&usart1, (uint8_t*)&buffer_tx_8bits, string_length);
        while(usart_status(&usart1, false) != ERROR_OK){}
    }

    usart_uninitialization(&usart1);
    return error;
}
