#include <inttypes.h>
#include <stdio.h>

#include "adapter/logging.h"
#include "adapter/task.h"

LOG_INIT(main)

void app_main(void)
{
    LOG_INFO("Hello world From ESP32!");

    while (1)
    {
        LOG_INFO("[ESP32] Running...\n");
        Task_SleepMs(1000);
    }
}
