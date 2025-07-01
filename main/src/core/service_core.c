
#include "core/service_core.h"

#include "adapter/logging.h"
#include "adapter/wifi.h"

#include "console/console.h"
#include "core/build_config.h"

LOG_INIT(core)

static void Core_Init(Service *service)
{
    WiFi_Init(service->queue);

    WiFiNetwork network = {.ssid = CONFIG_WIFI_SSID, .passphrase = CONFIG_WIFI_PASSWORD};
    WiFi_StartSta(&network);
}

static void Core_HandleEvent(Service *service, uint32_t event, uint32_t payload)
{
    LOG_VERBOSE("received event %" PRIu32 " %" PRIu32, event, payload);

    if (event == DRIVER_WIFI_EVENT_STA_GOT_IP)
    {
        LOG_INFO("Connection Up");
        Service_Start(SERVICE_NETWORK_CONSOLE);
    }
}

SERVICE_DEFINE(SERVICE_CORE){
    .name = "CORE",
    .handler = Core_HandleEvent,
    .setupHook = Core_Init,
    .timeoutMs = 500,
    .taskStackSize = 4096,
};