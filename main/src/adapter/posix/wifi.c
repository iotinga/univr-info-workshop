#include "adapter/wifi.h"

#include <string.h>

#include "adapter/logging.h"

#include "core/defines.h"

LOG_INIT(wifi)

ErrorCode WiFi_Init(Queue *eventQueue)
{
    return SUCCESS;
}

ErrorCode WiFi_StartSta(const WiFiNetwork *network)
{
    return SUCCESS;
}

ErrorCode WiFi_StopSta()
{
    return SUCCESS;
}
