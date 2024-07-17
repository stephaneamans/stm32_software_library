/*
 * error.h
 *
 * Created on: Mar 25, 2021
 * Author: Stephane Amans
 */
#ifndef ERROR_H_
#define ERROR_H_

typedef enum{
    ERROR_OK                      = 0,
    ERROR_BUSY                    = 1,
    ERROR_BUSY_RX                 = 2,
    ERROR_BUSY_TX                 = 3,
    ERROR_WRONG_VALUE             = 4,
    ERROR_WRONG_CLOCK_SET         = 5,
    ERROR_DRIVER_NOT_INITIALIZED  = 6,
	ERROR_WRITTEN_VALUE_CORRUPTED = 7,

    ERROR_USART_NOT_READY_TO_SEND = 10,
    ERROR_USART_NO_DATA_RECEIVED  = 11,
    ERROR_USART_PARITY            = 12,
    ERROR_USART_FRAMING           = 13,
    ERROR_USART_NOISE             = 14,
    ERROR_USART_OVERRUN           = 15,

    ERROR_SPI_PERIPH_UNKNOWN      = 20,
    ERROR_SPI_OVERUN              = 21,
    ERROR_SPI_MODE                = 22,
    ERROR_SPI_CRC                 = 23,
    ERROR_SPI_UNDERUN             = 24,

    ERROR_DMA_ERROR               = 29,

    ERROR_QUEUE_FULL              = 30,
    ERROR_QUEUE_EMPTY             = 31,

}t_error_handling;

#endif /* ERROR_H_ */
