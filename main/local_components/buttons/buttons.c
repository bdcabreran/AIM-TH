#include "buttons.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "event_router.h"

/**
 * @brief The GPIOs below can be used only as input, does not gave internal pull-up/pull-down
 * GPIO 34
 * GPIO 35 
 * GPIO 36
 * GPIO 39
 */

#define BUTTON4_GPIO    (39) 
#define BUTTON3_GPIO    (36) 
#define BUTTON2_GPIO    (35) 
#define BUTTON1_GPIO    (34) 


#define DEBOUNCE_TIME_MS    100
static QueueHandle_t button_evt_queue;
static const char* TAG = "button";


typedef enum
{
    BTN_PRESSED = 0, 
    BTN_RELEASE
}button_state_t;

void button_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(button_evt_queue, &gpio_num, NULL);
}

static void button_notify_event(uint32_t gpio_num, uint32_t btn_state)
{
    event_t event = {0}; 
    event.src = TASK_BUTTONS;
    event.dest = TASK_AIM;
    event.id = EVENT_INVALID;
    event.payload.len = 0;

    switch (gpio_num)
    {
        // case BUTTON4_GPIO: { event.id = (btn_state == BTN_PRESSED) ? EVT_BUTTON_4_PRESSED : EVT_BUTTON_4_RELEASED; } break;
        // case BUTTON3_GPIO: { event.id = (btn_state == BTN_PRESSED) ? EVT_BUTTON_3_PRESSED : EVT_BUTTON_3_RELEASED; } break;
        // case BUTTON2_GPIO: { event.id = (btn_state == BTN_PRESSED) ? EVT_BUTTON_2_PRESSED : EVT_BUTTON_2_RELEASED; } break;
        // case BUTTON1_GPIO: { event.id = (btn_state == BTN_PRESSED) ? EVT_BUTTON_1_PRESSED : EVT_BUTTON_1_RELEASED; } break;
        case BUTTON4_GPIO: { event.id = (btn_state == BTN_PRESSED) ? EVT_BUTTON_4_PRESSED : EVENT_INVALID; } break;
        case BUTTON3_GPIO: { event.id = (btn_state == BTN_PRESSED) ? EVT_BUTTON_3_PRESSED : EVENT_INVALID; } break;
        case BUTTON2_GPIO: { event.id = (btn_state == BTN_PRESSED) ? EVT_BUTTON_2_PRESSED : EVENT_INVALID; } break;
        case BUTTON1_GPIO: { event.id = (btn_state == BTN_PRESSED) ? EVT_BUTTON_1_PRESSED : EVENT_INVALID; } break;
    
    default:
        break;
    }

    if(event.id != EVENT_INVALID)
        event_router_write(&event);

}

static void button_driver_init(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

    io_conf.pin_bit_mask = (1ULL << BUTTON1_GPIO) | (1ULL << BUTTON2_GPIO) | (1ULL << BUTTON3_GPIO) | (1ULL << BUTTON4_GPIO);
    gpio_config(&io_conf);

    button_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON1_GPIO, button_isr_handler, (void *)BUTTON1_GPIO);
    gpio_isr_handler_add(BUTTON2_GPIO, button_isr_handler, (void *)BUTTON2_GPIO);
    gpio_isr_handler_add(BUTTON3_GPIO, button_isr_handler, (void *)BUTTON3_GPIO);
    gpio_isr_handler_add(BUTTON4_GPIO, button_isr_handler, (void *)BUTTON4_GPIO);
}



void button_task(void* arg)
{
    button_driver_init();

    uint32_t gpio_num;
    uint32_t last_state = 1;
    uint32_t debounce_time = DEBOUNCE_TIME_MS / portTICK_PERIOD_MS;

    for (;;) {
        if (xQueueReceive(button_evt_queue, &gpio_num, portMAX_DELAY)) {
            uint32_t curr_state = gpio_get_level(gpio_num);
            if (curr_state != last_state) {
                vTaskDelay(debounce_time);
                curr_state = gpio_get_level(gpio_num);
                if (curr_state != last_state) {
                    last_state = curr_state;
                    ESP_LOGI(TAG, "Button %ld is %s", gpio_num, curr_state ? "released" : "pressed");
                    
                    // Handle button event here
                    button_notify_event(gpio_num, curr_state);
                }
            }
        }
    }
}


void button_init(void)
{
    xTaskCreate(button_task, "button_task", 2048*2, NULL, 10, NULL);
    ESP_LOGI(TAG, "Buttons Initialized");
}

