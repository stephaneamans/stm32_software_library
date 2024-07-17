/*
 * lld_usart.h
 *
 * Created on: Apr 12, 2019
 * Author: Stephane Amans
 */

#ifndef USART_H_
#define USART_H_

/* Include files:        */
#include <stdint.h>
#include <stdbool.h>

#include "driver_common.h"
#include "clock.h"
#include "dma.h"
#include "nvic.h"
#include "regbase_usart.h"

enum t_usart_data_length
{
    usart_data_8_bits,
    usart_data_9_bits
};

enum t_usart_parity
{
    usart_parity_none,
    usart_parity_even,
    usart_parity_odd
};

enum t_usart_stop_bit
{
    usart_one_stop_bit,
    usart_half_stop_bit,
    usart_two_stop_bit,
    usart_one_half_stop_bit
};

/* USART driver structure definition :        */
struct t_usart_driver
{
	uintptr_t base_address;
    enum t_peripheral peripheral;
    uint8_t instance;
    uint32_t baudrate;
    enum t_usart_data_length data_length;
    enum t_usart_parity parity;
    enum t_usart_stop_bit stop_bit;
    struct
    {
        bool active;
    	enum irq_priority priority;
    }irq;
    struct
    {
        bool active;
    	struct t_dma_channel_driver *tx_channel;
    	struct t_dma_channel_driver *rx_channel;
    }dma;
    struct t_usart_private *priv;
};


/** Pointer to send data via USART.
 * This will call static functions send_usart_poll, send_usart_irq
 * or send_usart_dma function of the configuration. This pointer is affected
 * at the init of the driver.
 *
 * \param usart: address of the USART driver.
 * \param data_buffer: Pointer to the data buffer to send.
 * \param data_length: Data length.
 *
* \return: t_error_handling code or ERROR_OK.
 *
 */


/** Pointer to receive data via USART.
 * This will call static functions receive_usart_poll, receive_usart_irq
 * or receive_usart_dma function of the configuration. This pointer is affected
 * at the init of the driver.
 *
 * \param usart: address of the USART driver.
 * \param data_buffer: Pointer to the data receive buffer.
 *
 * \return: t_error_handling code or ERROR_OK.
 *
 */


/* USARTx_CRx, Peripheral configuration register group definition:        */

/* DMA bits configuration :             */
#define DMA_RX_ENABLE           0x01
#define DMA_TX_ENABLE           0x02
#define DMA_RX_TX_ENABLE        0x03


/* Functions prototypes:                */

/** Configure USART (baudrate, interruptions, mode, ...):
 *
 * \param driver: Driver structure
 * \param config: Configuration structure.
 *
 * \return: void
 *
 */
void usart_initialization(struct t_usart_driver *config);

t_error_handling usart_receive(struct t_usart_driver *driver, uint8_t *data_buffer, uint32_t data_length);

t_error_handling usart_transmit(struct t_usart_driver *driver, uint8_t *data_buffer, uint32_t data_length);

t_error_handling usart_status(struct t_usart_driver *driver, bool read);

void usart1_dma_ch4_irq_management(struct t_dma_status *dma_status);

void usart1_dma_ch5_irq_management(struct t_dma_status *dma_status);

void usart_uninitialization(struct t_usart_driver *driver);

#endif /* USART_H_ */
