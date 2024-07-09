#include "light.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#define LIGHT_GPIO_PIN GPIO_NUM_4 //! TODO: Update this

static const char *TAG = "LIGHT";

void light_init(void)
{
    gpio_reset_pin(LIGHT_GPIO_PIN); // Configure relay GPIO pin as output
    gpio_set_direction(LIGHT_GPIO_PIN, GPIO_MODE_OUTPUT);
    ESP_LOGI(TAG, "Light Initialized");
}

void light_on(void)
{
    gpio_set_level(LIGHT_GPIO_PIN, 1);
    ESP_LOGI(TAG, "Light ON");
}

void light_off(void)
{
    gpio_set_level(LIGHT_GPIO_PIN, 0);
    ESP_LOGI(TAG, "Light OFF");
}

void light_on_with_timeout(uint32_t tick_ms)
{
    gpio_set_level(LIGHT_GPIO_PIN, 1);
    ESP_LOGI(TAG, "Light ON - [%d ms]", tick_ms);
    vTaskDelay(tick_ms / portTICK_PERIOD_MS);
}
