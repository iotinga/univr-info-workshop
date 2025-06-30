#pragma once

typedef enum {
    SUCCESS = 0,
    FAILURE = -1,
    IGNORED = -2,

    ERROR_INVALID_STATE = 1,
    ERROR_INVALID_ARGUMENTS,
    ERROR_NO_MEMORY,
    ERROR_ASSERTION_FAILED,
    ERROR_UNREACHABLE_CODE,
    ERROR_RESOURCE_NOT_AVAILABLE,
    ERROR_CRC,
    ERROR_TIMEOUT,
    ERROR_NOT_IMPLEMENTED,
    ERROR_NOT_FOUND,

    ERROR_OS = 0x1000,

    ERROR_NETWORK = 0x2000,

    ERROR_BLE = 0x3000,

    ERROR_MQTT = 0x4000,
} ErrorCode;

#define CASE(x)                                                                                                        \
    case x:                                                                                                            \
        return #x;

static inline const char *ErrorToString(ErrorCode error)
{
    switch (error)
    {
        CASE(SUCCESS)
        CASE(FAILURE)
        CASE(IGNORED)
        CASE(ERROR_INVALID_STATE)
        CASE(ERROR_INVALID_ARGUMENTS)
        CASE(ERROR_NO_MEMORY)
        CASE(ERROR_ASSERTION_FAILED)
        CASE(ERROR_UNREACHABLE_CODE)
        CASE(ERROR_RESOURCE_NOT_AVAILABLE)
        CASE(ERROR_CRC)
        CASE(ERROR_TIMEOUT)
        CASE(ERROR_NOT_IMPLEMENTED)
        CASE(ERROR_NOT_FOUND)
        CASE(ERROR_OS)
        CASE(ERROR_BLE)
        CASE(ERROR_NETWORK)
        CASE(ERROR_MQTT)
    }

    return "UNKNOWN";
}

#undef CASE
