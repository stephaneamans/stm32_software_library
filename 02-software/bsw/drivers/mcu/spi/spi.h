/*
 * lld_spi.h
 *
 * Created on: Jun 25, 2021
 * Author: Stephane Amans
 *
 */

#ifndef LLD_SPI_H_
#define LLD_SPI_H_

/* Include files:        */
#include <stdint.h>
#include <stdbool.h>

#include "driver_common.h"
#include "dma.h"
#include "nvic.h"
#include "gpio.h"

/* Enum used to configure data type 8 or 16 bits. */
enum t_spi_data_length
{
    spi_data_8_bits,
    spi_data_16_bits
};

/* Enum used to configure the frame direction. */
enum t_spi_frame_direction
{
    spi_frame_msb_first,
    spi_frame_lsb_first
};

/* Enum used to configure the SPI clock phase synchronization. */
enum t_spi_clock_phase
{
    spi_clk_first,
    spi_clk_second
};

/* Enum used to configure the SPI clock polarity synchronization. */
enum t_spi_clock_polarity
{
    spi_clk_rising,
    spi_clk_falling
};

/* SPI data structure definition, this structure is used to encapsulate the data parameters. */
struct t_spi_data
{
    uint16_t *write_buffer;
    uint16_t *read_buffer;
    uint32_t length;
};

/* SPI slave structure definition, this structure is used to encapsulate the slaves parameters .*/
struct t_spi_slave
{
    struct t_gpio_driver *cs;
    uint16_t freq_khz;
    enum t_spi_data_length data_length;
    enum t_spi_frame_direction frame_direction;
    enum t_spi_clock_phase clock_phase;
    enum t_spi_clock_polarity clock_polarity;
    uint8_t id;
};

/* SPI driver structure definition, it contains the global configuration setting fields of the SPI. */
struct t_spi_driver
{
    uintptr_t base_address;
    enum t_peripheral peripheral;
    uint8_t instance;
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
    struct t_spi_private *priv;
};

/* Functions prototypes:                */

/** Callback function called on a DMA transfer complete IRQ.
 *  Release the slave's CS pin on a transfer complete event.
 *
 * \param dma_status: Pointer to the DMA status structure.
 *
 * \return : Error code or ERROR_OK.
 *
 */
void spi_dma_ch2_irq_management(struct t_dma_status *dma_status);

/** Register a SPI slave into the internal database.
 *
 * \param slave: Pointer to the slave configuration.
 *
 * \return : void.
 *
 */
void spi_slave_register(struct t_spi_slave *slave);

/** Perform a SPI transfer on a specific slave.
 *
 * \param driver: Pointer to the SPI driver.
 * \param slave: Pointer to the slave configuration.
 * \param data: Pointer to the data structure.
 *
 * \return : Error code or ERROR_OK.
 *
 */
t_error_handling spi_transfer(struct t_spi_driver *driver, struct t_spi_slave *slave, struct t_spi_data *data);

/** Return the transfer status on a specific slave.
 *
 * \param driver: Pointer to the SPI driver.
 * \param slave: Pointer to the slave structure.
 *
 * \return : Error code or ERROR_OK.
 *
 */
t_error_handling spi_transfer_status(struct t_spi_driver *driver, struct t_spi_slave *slave);

/** SPI uninitialization, reset the module and clear the private structure.
 *
 * \param driver: Pointer to the SPI driver structure.
 *
 * \return void.
 *
 */
void spi_uninitialization(struct t_spi_driver *driver);

/** SPI initialization.
 *
 * \param driver: Pointer to the SPI driver structure.
 *
 * \return void.
 *
 */
void spi_initialization(struct t_spi_driver *driver);

#endif /* SPI_H_ */
