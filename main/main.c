#include <inttypes.h>
#include <stdio.h>

#include "adapter/logging.h"
#include "adapter/task.h"

#include "core/service.h"
#include "core/service_core.h"
#include "hmi/hmi.h"

LOG_INIT(main)

void app_main(void)
{
    LOG_INFO("Hello world From ESP32!");

    Service_Start(SERVICE_CORE);
    Service_Start(SERVICE_HMI);

    while (1)
    {
        LOG_INFO("[ESP32] Running...\n");
        Task_SleepMs(1000);
    }
}
