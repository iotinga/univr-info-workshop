#include "adapter/btn.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "adapter/queue.h"

#include "core/defines.h"
#include "core/service.h"
#include "hmi/hmi.h"

LOG_INIT(btn)

#define BOOT_BUTTON_GPIO      0
#define ESP_INTR_FLAG_DEFAULT 0

static struct {
    QueueHandle_t gpioEventQueue;
} ctx;

static void IRAM_ATTR Btn_GpioIsrHandler(void *arg)
{
    ServiceEvent event = {.event = HMI_EVENT_BTN_PRESS};
    ON_FALSE_REBOOT(pdPASS == xQueueSendFromISR(ctx.gpioEventQueue, &event, NULL));
}

ErrorCode Btn_Init(Queue *eventQueue)
{
    ctx.gpioEventQueue = (QueueHandle_t)eventQueue;

    gpio_config_t btn_conf = {
        .intr_type = GPIO_INTR_NEGEDGE, // fronte di discesa (pressione)
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pin_bit_mask = BIT(BOOT_BUTTON_GPIO),
    };

    CHECK_ERROR(gpio_config(&btn_conf));
    CHECK_ERROR(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
    CHECK_ERROR(gpio_isr_handler_add(BOOT_BUTTON_GPIO, Btn_GpioIsrHandler, NULL));

    return SUCCESS;
}