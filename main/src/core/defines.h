#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "adapter/logging.h"
#include "adapter/task.h"

#include "core/error.h"

#define STR(x)  #x
#define XSTR(x) STR(x)

#define PACKED     __attribute__((packed))
#define UNUSED     __attribute__((unused))
#define FALLTROUGH __attribute__((fallthrough))
#define INLINE     static inline __attribute__((always_inline))

#define CHECK_ERROR(e)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        ErrorCode __error = (e);                                                                                       \
        if (__error)                                                                                                   \
            return __error;                                                                                            \
    }                                                                                                                  \
    while (0)
#define ON_FALSE_RETURN(condition, error)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(condition))                                                                                              \
            return (error);                                                                                            \
    }                                                                                                                  \
    while (0)
#define ON_NULL_RETURN(value, error) ON_FALSE_RETURN(value != NULL, error)

/** Reboots the device if an error occurs. Use this in places where an error is not expected.
 * If an error cannot be handled (e.g. memory allocation fails, cannot write to flash) it's
 * best to restart the device */
#define ON_ERROR_REBOOT(error)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        ErrorCode __error = (error);                                                                                   \
        if (__error != SUCCESS)                                                                                        \
        {                                                                                                              \
            LOG_ERROR("unexpected error %d in %s: " #error, __error, Task_GetName(NULL));                              \
            abort();                                                                                                   \
        }                                                                                                              \
    }                                                                                                                  \
    while (0)

/** Reboots the device if the argument is false */
#define ON_FALSE_REBOOT(x) ON_ERROR_REBOOT((x) ? SUCCESS : FAILURE)

/** Reboots the device if the argument is boot */
#define ON_NULL_REBOOT(x) ON_FALSE_REBOOT((x) != NULL)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef BIT
#undef BIT
#endif
#define BIT(x)          (1lu << (x))
#define BIT_SET(v, b)   v |= BIT(b)
#define BIT_CLR(v, b)   v &= ~BIT(b)
#define BIT_ISSET(v, b) ((v & BIT(b)) != 0)
#define BIT_SETE(v, b, e)                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        if (e)                                                                                                         \
            BIT_SET(v, b);                                                                                             \
        else                                                                                                           \
            BIT_CLR(v, b);                                                                                             \
    }                                                                                                                  \
    while (0)

#ifdef ARRAY_SIZE
#undef ARRAY_SIZE
#endif
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define LOG_TAG "defines"

/**
 * Allocates memory like malloc, but checks for memory allocation failure
 */
static inline void *xmalloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    void *memory = malloc(size);
    ON_NULL_REBOOT(memory);

    return memory;
}

/**
 * Allocates memory like malloc, but checks for memory allocation failure
 */
static inline void *xcalloc(size_t num, size_t size)
{
    if (size == 0 || num == 0)
    {
        return NULL;
    }

    void *memory = calloc(num, size);
    ON_NULL_REBOOT(memory);

    return memory;
}

#undef LOG_TAG

typedef struct {
    bool dynamic;
    size_t length;
    uint8_t *buffer;
} Buffer;

/**
 * Allocates a buffer dynamically with the specified size
 *
 * \param buffer the pointer to the buffer to allocate
 * \param size the size of the buffer to allocate
 */
static inline void Buffer_Alloc(Buffer *buffer, size_t size)
{
    buffer->buffer = (uint8_t *)xcalloc(1, size);
    buffer->length = size;
    buffer->dynamic = true;
}

/**
 * Deallocates a previously allocated buffer, only if dynamically
 * allocated.
 *
 * \param buffer the buffer to deallocate.
 */
static inline void Buffer_Free(Buffer *buffer)
{
    if (buffer->dynamic)
    {
        free(buffer->buffer);
        buffer->buffer = NULL;
        buffer->length = 0;
        buffer->dynamic = false;
    }
}

typedef struct {
    bool dynamic;
    size_t length;
    const uint8_t *buffer;
} CBuffer;
