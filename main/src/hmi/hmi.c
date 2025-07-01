#include "hmi/hmi.h"

#include "adapter/btn.h"
#include "adapter/led.h"
#include "adapter/logging.h"

LOG_INIT(hmi)

static struct {
    RGBW color;
    int count;
} ctx;

static void Hmi_Init(Service *service)
{
    Btn_Init(service->queue);
    Led_Init();
}

static void Hmi_HandleEvent(Service *service, uint32_t event, uint32_t payload)
{
    LOG_VERBOSE("received event %" PRIu32 " %" PRIu32, event, payload);

    if (event == HMI_EVENT_BTN_PRESS)
    {
        if (ctx.count == 0)
        {
            ctx.color = (RGBW){.r = 0, .g = 0, .b = 0, .w = 0};
            ctx.count += 1;
        }
        else if (ctx.count == 1)
        {
            ctx.color = (RGBW){.r = 255, .g = 0, .b = 0, .w = 0};
            ctx.count += 1;
        }
        else if (ctx.count == 2)
        {
            ctx.color = (RGBW){.r = 0, .g = 255, .b = 0, .w = 0};
            ctx.count += 1;
        }
        else
        {
            ctx.color = (RGBW){.r = 0, .g = 0, .b = 255, .w = 0};
            ctx.count = 0;
        }

        Led_SetLedColor(&ctx.color);
    }
}

SERVICE_DEFINE(SERVICE_HMI){
    .name = "HMI",
    .handler = Hmi_HandleEvent,
    .setupHook = Hmi_Init,
    .timeoutMs = 1000,
    .taskStackSize = 4096,
};
