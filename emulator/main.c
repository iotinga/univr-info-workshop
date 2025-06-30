#include <inttypes.h>
#include <stdio.h>

#include "adapter/logging.h"
#include "adapter/task.h"

LOG_INIT(main)

int main(void)
{
    LOG_INFO("Hello world From EMULATOR!");

    while (1)
    {
        LOG_DEBUG("[EMU] Running...");
        Task_SleepMs(1000);
    }

    return 0;
}