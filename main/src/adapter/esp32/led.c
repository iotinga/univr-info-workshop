#include "adapter/led.h"

#include "core/defines.h"
#include "core/error.h"
#include "led_strip.h"

#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000) // 10 MHz
#define LED_STRIP_GPIO       48

static struct {
    led_strip_handle_t led_strip;
} ctx;

ErrorCode Led_Init(void)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO,
        .max_leds = 1,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = LED_STRIP_RMT_RES_HZ,
        .flags.with_dma = false,
    };

    CHECK_ERROR(led_strip_new_rmt_device(&strip_config, &rmt_config, &ctx.led_strip));
    CHECK_ERROR(led_strip_clear(ctx.led_strip));

    return SUCCESS;
}

ErrorCode Led_SetLedColor(const RGBW *color)
{
    if (ctx.led_strip == NULL)
    {
        return ERROR_RESOURCE_NOT_AVAILABLE;
    }

    CHECK_ERROR(led_strip_set_pixel(ctx.led_strip, 0, color->r, color->g, color->b));
    CHECK_ERROR(led_strip_refresh(ctx.led_strip));

    return SUCCESS;
}
