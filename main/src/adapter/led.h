#pragma once

#include <stdint.h>

#include "core/error.h"

/**
 * Color expressed in RGB with white
 */
typedef struct {
    /** Red channel 0-255b */
    uint8_t r;

    /** Green channel 0-255 */
    uint8_t g;

    /** Blue channel 0-255 */
    uint8_t b;

    /** White channel 0-255 */
    uint8_t w;
} RGBW;

/**
 *
 */
ErrorCode Led_Init(void);

/**
 * Sets the state of the RGBW leds
 *
 * \param driver driver instance
 * \param value rgbw value to set
 * \returns SUCCESS or ERROR_RESOURCE_NOT_AVAILABLE if driver not initialized or other failure code
 */
ErrorCode Led_SetLedColor(const RGBW *color);