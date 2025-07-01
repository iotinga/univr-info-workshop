#include "adapter/wifi.h"

#include "esp_wifi.h"
#include "nvs_flash.h"

#include "adapter/logging.h"

#include "core/defines.h"
#include "core/service.h"

LOG_INIT(wifi)

#define CONNECT_MAXIMUM_RETRY 5

static struct {
    Queue *queue;
    bool isConnected;
    uint32_t connectionRetryCount;
} ctx;

static void WiFi_SendEvent(uint32_t event, uint32_t payload)
{
    ServiceEvent serviceEvent = {event, payload};
    ON_ERROR_REBOOT(Queue_Send(ctx.queue, &serviceEvent));
}

static void Wifi_EventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
            LOG_INFO("try connect to AP");
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            LOG_INFO("connection failure");
            ctx.isConnected = false;
            if (ctx.connectionRetryCount < CONNECT_MAXIMUM_RETRY)
            {
                ctx.connectionRetryCount += 1;
                LOG_INFO("retry connect to the AP");
                esp_wifi_connect();
            }
            break;
        case WIFI_EVENT_STA_CONNECTED:
            LOG_INFO("connected to AP");
            ctx.isConnected = true;
            ctx.connectionRetryCount = 0;
            break;
        default:
            LOG_INFO("unhandled wifi event: %ld", event_id);
            break;
        }
    }

    if (event_base == IP_EVENT)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
            LOG_INFO("got ip IPv4:" IPSTR, IP2STR(&event->ip_info.ip));
            WiFi_SendEvent(DRIVER_WIFI_EVENT_STA_GOT_IP, 0);
            break;
        case IP_EVENT_STA_LOST_IP:
            WiFi_SendEvent(DRIVER_WIFI_EVENT_STA_LOST_IP, 0);
            break;

        case IP_EVENT_AP_STAIPASSIGNED:
            WiFi_SendEvent(DRIVER_WIFI_EVENT_AP_IP_ASSIGNED, 0);
            break;

        default:
            LOG_INFO("unhandled ip event: %ld", event_id);
            break;
        }
    }
}

ErrorCode WiFi_Init(Queue *eventQueue)
{
    ctx.queue = eventQueue;

    CHECK_ERROR(nvs_flash_init());

    CHECK_ERROR(esp_event_loop_create_default());
    CHECK_ERROR(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi_EventHandler, NULL, NULL));
    CHECK_ERROR(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &Wifi_EventHandler, NULL, NULL));

    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    return SUCCESS;
}

ErrorCode WiFi_StartSta(const WiFiNetwork *network)
{
    wifi_config_t staConfig = {0};
    memcpy(staConfig.sta.ssid, network->ssid, WIFI_SSID_MAX_LENGTH);
    memcpy(staConfig.sta.password, network->passphrase, WIFI_PASSWORD_MAX_LENGTH);

    LOG_INFO("start STA - SSID: %s PWD: %s", staConfig.sta.ssid, staConfig.sta.password);

    CHECK_ERROR(esp_wifi_set_mode(WIFI_MODE_STA));
    CHECK_ERROR(esp_wifi_set_config(WIFI_IF_STA, &staConfig));
    CHECK_ERROR(esp_wifi_start());

    return SUCCESS;
}

ErrorCode WiFi_StopSta(void)
{
    if (ctx.isConnected)
    {
        CHECK_ERROR(esp_wifi_disconnect());
    }

    CHECK_ERROR(esp_wifi_stop());

    return SUCCESS;
}
