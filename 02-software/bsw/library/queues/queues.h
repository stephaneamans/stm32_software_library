/*
 * queues.h
 *
 * Created on: Jun 2, 2020
 * Author: Stephane Amans
 */
#ifndef QUEUES_H_
#define QUEUES_H_

/* Include files:        */
#include <stdint.h>
#include <string.h>
#include "error.h"

/** Documentation
 *
 * Define queue prototype
 * \param NAME: Queue name, the name is given to functions and structure.
 * \param BUFFER_TYPE: Data type of the buffer.
 * \param BUFFER_LENGTH: Buffer length.
 * DEFINE_QUEUE(NAME, BUFFER_TYPE, BUFFER_LENGTH)
 *
 * Implement queue functions and structure.
 * \param NAME: Queue name, the name is given to functions and structure.
 * \param BUFFER_TYPE: Data type of the buffer.
 * \param BUFFER_LENGTH: Buffer length.
 * IMPLEMENT_QUEUE(NAME, BUFFER_TYPE, BUFFER_LENGTH)
 *
 * Initialize the queue.
 * \param queue: Pointer to the queue.
 * \return: Error code or OK.
 * t_error_handling queue_init(t_queue *queue);
 *
 * Write the queue.
 * \param queue: Pointer to the queue.
 * \param data: Pointer to the data to write into the queue.
 * \return: Error code or OK if the buffer is not full.
 * t_error_handling queue_write(t_queue *queue, uint8_t *data);
 *
 * Read the queue.
 * \param queue: Pointer to the queue.
 * \param data: Pointer to the location to store the data read.
 * \return: Error code or OK if the buffer is not empty.
 * t_error_handling queue_read(t_queue *queue, uint8_t *data);
 *
 */

#define DEFINE_QUEUE(NAME, BUFFER_TYPE, BUFFER_LENGTH)                        \
                                                                              \
typedef struct                                                                \
{                                                                             \
    uint32_t read_ptr;                                                        \
    uint32_t write_ptr;                                                       \
    BUFFER_TYPE buffer[BUFFER_LENGTH];                                        \
}t_##NAME;                                                                    \
                                                                              \
t_error_handling NAME##_init(t_##NAME *queue);                                \
t_error_handling NAME##_write(t_##NAME *queue, BUFFER_TYPE *data);            \
t_error_handling NAME##_read(t_##NAME *queue, BUFFER_TYPE *data);             \

#define IMPLEMENT_QUEUE(NAME, BUFFER_TYPE, BUFFER_LENGTH)                     \
                                                                              \
static t_error_handling NAME##_is_not_full(t_##NAME *queue);                  \
static t_error_handling NAME##_is_not_empty(t_##NAME *queue);                 \
                                                                              \
t_error_handling NAME##_init(t_##NAME *queue)                                 \
{                                                                             \
    queue->write_ptr = 0;                                                     \
    queue->read_ptr = 0;                                                      \
    return OK;                                                                \
}                                                                             \
                                                                              \
t_error_handling NAME##_write(t_##NAME *queue, BUFFER_TYPE *data)             \
{                                                                             \
    t_error_handling error = ERROR_QUEUE_FULL;                                \
    if(NAME##_is_not_full(queue) == OK)                                       \
    {                                                                         \
        memcpy(&queue->buffer[queue->write_ptr], data, sizeof(BUFFER_TYPE));  \
        queue->write_ptr = (queue->write_ptr + 1) % BUFFER_LENGTH;            \
        error = OK;                                                           \
    }                                                                         \
    return error;                                                             \
}                                                                             \
                                                                              \
t_error_handling NAME##_read(t_##NAME *queue, BUFFER_TYPE *data)              \
{                                                                             \
    t_error_handling error = ERROR_QUEUE_EMPTY;                               \
    if(NAME##_is_not_empty(queue) == OK)                                      \
    {                                                                         \
        memcpy(data, &queue->buffer[queue->read_ptr], sizeof(BUFFER_TYPE));   \
        queue->read_ptr = (queue->read_ptr + 1) % BUFFER_LENGTH;              \
        error = OK;                                                           \
    }                                                                         \
    return error;                                                             \
}                                                                             \
                                                                              \
static t_error_handling NAME##_is_not_full(t_##NAME *queue)                   \
{                                                                             \
    t_error_handling is_not_full = OK;                                        \
    if(queue->write_ptr == ((queue->read_ptr - 1) % BUFFER_LENGTH))           \
    {                                                                         \
        is_not_full = ERROR_QUEUE_FULL;                                       \
    }                                                                         \
    return is_not_full;                                                       \
}                                                                             \
                                                                              \
static t_error_handling NAME##_is_not_empty(t_##NAME *queue)                  \
{                                                                             \
    t_error_handling is_not_empty = OK;                                       \
    if(queue->write_ptr == queue->read_ptr)                                   \
    {                                                                         \
        is_not_empty = ERROR_QUEUE_EMPTY;                                     \
    }                                                                         \
    return is_not_empty;                                                      \
}                                                                             \

#endif /* QUEUES_H_ */
