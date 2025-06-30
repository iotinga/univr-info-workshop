#pragma once

#include <stddef.h>
#include <stdint.h>

#include "core/error.h"

static const uint64_t QUEUE_MAX_TIMEOUT = UINT64_MAX;

/**
 * \file osal_queue.h
 * Interface for a message queue
 */

/** Opaque queue handle */
typedef struct Queue Queue;

/**
 * Creates a new message queue
 *
 * \param queueSize maximum number of elements in the queue
 * \param elementSize size of each element
 * \return handle to the queue, or NULL on failure
 */
Queue *Queue_New(size_t queueSize, size_t elementSize);

/**
 * Delete an already created queue
 *
 * \param queue queue handle
 */
void Queue_Delete(Queue *queue);

/**
 * Sends a message in the queue
 *
 * \param queue queue handle
 * \param message message to send
 * \return SUCCESS on success, otherwise failure
 */
ErrorCode Queue_Send(Queue *queue, void *message);

/**
 * Receives a message in the queue, waiting for maximum timeoutMs
 * before returning a FAILURE if no message is available
 *
 * \param queue queue handle
 * \param message message to receive
 * \param timeoutMs how many milliseconds to wait for a message to arrive. If 0 non blocking.
 * \return SUCCESS if a message is received
 */
ErrorCode Queue_Receive(Queue *queue, void *message, uint64_t timeoutMs);