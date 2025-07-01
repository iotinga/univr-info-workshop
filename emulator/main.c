#include <inttypes.h>
#include <stdio.h>

#include "adapter/logging.h"
#include "adapter/task.h"

#include "console/console.h"
#include "core/service.h"
#include "core/service_core.h"
#include "hmi/hmi.h"

LOG_INIT(main)

int main(void)
{
    LOG_INFO("Hello world From EMULATOR!");

    Service_Start(SERVICE_CORE);
    Service_Start(SERVICE_HMI);
    Service_Start(SERVICE_NETWORK_CONSOLE);

    while (1)
    {
        LOG_DEBUG("[EMU] Running...");
        Task_SleepMs(1000);
    }

    return 0;
}