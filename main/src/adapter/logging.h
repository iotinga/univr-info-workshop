#pragma once

#include <inttypes.h>

/**
 * Macro used to inizialize the logger. Use it globally on a file,
 * the name of the logger is usually the file name without the .c extension
 */
#define LOG_INIT(name) static __attribute__((unused)) const char LOG_TAG[] = #name;

#if __has_include(<esp_log.h>)
#include <esp_log.h>

/**
 * Logs errors. An error is a condition that compromises the outcome
 * of an operation (i.e. a failure).
 * Examples: parsing of something fails, a write to the flash fails, etc.
 */
#define LOG_ERROR(fmt, ...) ESP_LOGE(LOG_TAG, fmt, ##__VA_ARGS__)

/**
 * Logs warnings. A warning is a condition that doesn't compromise the
 * outcome of the operation, i.e. what it was trying to achieve it's
 * still achieved, but there is something that is not completely right
 * that necessitates of attention.
 * Examples: an operations taking too much time, an unexpected event that arrives,
 * a connection that is reset, an operation that succeeds after a retry.
 */
#define LOG_WARNING(fmt, ...) ESP_LOGW(LOG_TAG, fmt, ##__VA_ARGS__)

/**
 * Logs informations. Informations shall give the developer an idea
 * of what the device is doing, without beeing too much verbose.
 * Examples: state changes, important events, situations that needs to be
 * taken into account.
 */
#define LOG_INFO(fmt, ...) ESP_LOGI(LOG_TAG, fmt, ##__VA_ARGS__)

/**
 * Logs debug information. Debug information is technical details that
 * are useful to know in details the state of the device and what it's
 * doing, but without too much verbosity (e.g. no recurring logs).
 * Examples: entering/exiting a particular functions, writing the value
 * of state variables, writing responses/requests to a server, etc.
 */
#define LOG_DEBUG(fmt, ...) ESP_LOGD(LOG_TAG, fmt, ##__VA_ARGS__)

/**
 * Logs verbose information. This includes any log that is not accounted
 * in the previous levels. This level may include recurring log output.
 * Normally this output is not enabled beside special conditions.
 * Examples: printing a line each time a message is processed in a service.
 */
#define LOG_VERBOSE(fmt, ...) ESP_LOGV(LOG_TAG, fmt, ##__VA_ARGS__)

#else

#include <stdio.h>
#include <sys/time.h>

static inline uint64_t __getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/* provide macro that don't rely on ESP-IDF for unit tests */
#if LOG_LEVEL >= 1
#define LOG_ERROR(fmt, ...) fprintf(stderr, "E (%" PRIu64 ") %s: " fmt "\n", __getTime(), LOG_TAG, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)
#endif

#if LOG_LEVEL >= 2
#define LOG_WARNING(fmt, ...) fprintf(stderr, "W (%" PRIu64 ") %s: " fmt "\n", __getTime(), LOG_TAG, ##__VA_ARGS__)
#else
#define LOG_WARNING(fmt, ...)
#endif

#if LOG_LEVEL >= 3
#define LOG_INFO(fmt, ...) fprintf(stderr, "I (%" PRIu64 ") %s: " fmt "\n", __getTime(), LOG_TAG, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

#if LOG_LEVEL >= 4
#define LOG_DEBUG(fmt, ...) fprintf(stderr, "D (%" PRIu64 ") %s: " fmt "\n", __getTime(), LOG_TAG, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#if LOG_LEVEL >= 5
#define LOG_VERBOSE(fmt, ...) fprintf(stderr, "V (%" PRIu64 ") %s: " fmt "\n", __getTime(), LOG_TAG, ##__VA_ARGS__)
#else
#define LOG_VERBOSE(fmt, ...)
#endif

#endif /* _has_include(<esp_log.h>) */

/**
 * Initialize logging module
 */
void Logging_Init(void);

/**
 * Update the log level for a specified component
 *
 * @param tag log level tag, or NULL to set master level
 * @param level string to indicate level: ERROR, INFO, DEBUG, WARN. If NULL logs disabled.
 */
void Logging_SetLevel(const char *tag, const char *level);