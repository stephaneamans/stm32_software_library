/*
 * lld_dma.h
 *
 * Created on: Jun 8, 2020
 * Author: Stephane Amans
 *
 */

#ifndef LLD_DMA_H_
#define LLD_DMA_H_

/* Include files:        */
#include <stdbool.h>
#include <stdint.h>

#include "driver_common.h"

/* Memory or peripheral data length. */
enum t_dma_data_type
{
    dma_8_bits,
    dma_16_bits,
    dma_32_bits
};

/* DMA client structure definition :        */
struct t_dma_client
{
    bool memory_increment;
    bool peripheral_increment;
    bool read_from_memory;
    enum t_dma_data_type mem_data_type;
    uint16_t transfer_length;
    uintptr_t memory_address;
    enum t_dma_data_type periph_data_type;
    uintptr_t peripheral_address;
};

/* DMA status structure definition :        */
struct t_dma_status
{
    bool transfer_complete;
    bool half_transfer_complete;
    bool transfer_error;
};

/* DMA driver structure definition :        */
struct t_dma_driver
{
    uintptr_t base_address_dma;
    enum t_peripheral peripheral;
    uint16_t instance;
    struct t_dma_private *priv;
};

/* DMA channel driver structure definition :        */
struct t_dma_channel_driver
{
    uintptr_t base_address_dma_channel;
    uint8_t channel_number;
    bool mem2mem;
    uint8_t dma_priority_level;
    struct
    {
        uint8_t priority;
        bool transfer_complete;
        bool half_transfer_complete;
        bool transfer_error;
        void (*callback)(struct t_dma_status*);
    }irq;
    struct t_dma_channel_private *priv;
};

/** Get the DMA half transfer complete flag:
 *
 * \param driver: Pointer to the DMA channel driver structure.
 *
 * \return: bool, half transfer boolean, true if it is half complete.
 *
 */
bool dma_get_half_transfer_complete(struct t_dma_channel_driver *driver);

/** Get the DMA transfer complete flag:
 *
 * \param driver: Pointer to the DMA channel driver structure.
 *
 * \return: bool, half transfer boolean, true if it is complete.
 *
 */
bool dma_get_transfer_complete(struct t_dma_channel_driver *driver);

/** Get the DMA transfer error flag:
 *
 * \param driver: Pointer to the DMA channel driver structure.
 *
 * \return: bool, half transfer boolean, true if ithere was an error.
 *
 */
bool dma_get_transfer_error(struct t_dma_channel_driver *driver);


/** Set DMA parameter to prepare a new transfer
 *
 * \param driver: Pointer to the DMA channel driver structure.
 * \param client: Pointer to the DMA client parameters structure.
 * 
 * \return: void.
 *
 */
void dma_set_transfer(struct t_dma_channel_driver *driver, struct t_dma_client *client);

/** Start DMA transfer:
 *
 * \param driver: Pointer to the DMA channel driver structure.
 *
 * \return: void.
 *
 */
void dma_start_transfer(struct t_dma_channel_driver *driver);

/** Stop DMA transfer:
 *
 * \param driver: Pointer to the DMA channel driver structure.
 *
 * \return: void.
 *
 */
void dma_stop_transfer(struct t_dma_channel_driver *driver);

/** Get DMA transfer status:
 *     - transfer complete flag,
 *     - half transfer complete flag,
 *     - transfer error flag,
 *
 * \param driver: Pointer to the DMA channel driver structure.
 * \param client: Pointer to the DMA client parameters structure.
 * 
 * \return: void.
 *
 */
void dma_transfer_status(struct t_dma_channel_driver *driver, struct t_dma_status *dma_status);

/** Configure DMA:
 *
 * \param config         : Pointer to the DMA driver structure;
 * \param channel_config : Pointer to the DMA channel driver structure.
 *
 * \return: void.
 *
 */
void dma_initialization(struct t_dma_driver *config, struct t_dma_channel_driver *channel_config);

#endif /* LLD_DMA_H_ */
