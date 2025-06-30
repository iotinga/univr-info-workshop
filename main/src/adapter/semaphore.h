/**
 * @file semaphore.h
 * @brief This file manages binary semaphores on the firmware
 */

#pragma once

#include <stdint.h>

#include "core/error.h"

/** Opaque semaphore handle */
typedef struct Semaphore Semaphore;

/**
 * @brief Creates a new semaphore
 *
 * @returns the semaphore handle
 */
Semaphore *Semaphore_New(void);

/**
 * @brief Take the specified semaphore
 *
 * @param semaphore semaphore handle
 * @param timeoutMs how much to wait for the semaphore to be available
 * @returns SUCCESS if semaphore taken, otherwise ERROR_TIMEOUT
 */
ErrorCode Semaphore_Take(Semaphore *semaphore, uint64_t timeoutMs);

/**
 * @brief Give the specified semaphore
 *
 * @param semaphore semaphore handle
 * @returns SUCCESS on success, otherwise ERROR_OS
 */
ErrorCode Semaphore_Give(Semaphore *semaphore);

/**
 * @brief Frees the specified semaphore
 *
 * @param semaphore semaphore handle
 */
void Semaphore_Free(Semaphore *semaphore);