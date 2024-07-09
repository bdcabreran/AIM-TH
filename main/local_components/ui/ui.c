
#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"

/* Lvgl specific */
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"

#include "ui_wifi_screen.h"
#include "esp_timer.h"
// #include "ui_main_screen.h"

/**
 * @brief Display configuration 
 *  Open menuconfig -> Component Config -> LVGL TFT Display Controller -> 
 *  Select a display controller model. (ILI9341)
 *  TFT SPI Bus. (HSPI)
 *  Display orientation (Landscape)
 *  (320) TFT display width in pixels
 *  (240) TFT display height in pixels.
 *  Display Pin Assignments  --->   
 *      (13) GPIO for MOSI (Master Out Slave In) 
 *      [ ] Use MISO signal to control the display
 *      (14) GPIO for CLK (SCK / Serial Clock) 
 *      [*] Use CS signal to control the display 
 *      (15)    GPIO for CS (Slave Select)  
 *      [*] Use DC signal to control the display 
 *      (2)     GPIO for DC (Data / Command)  
 *      (4) GPIO for Reset 
 *      [ ] Enable control of the display backlight by using an GPIO.
 */


#define LV_TICK_PERIOD_MS    (10)
#define USE_TOUCH_PANEL      (1) 
#define USE_DOUBLE_DRAW_BUFF (1)
#define UI_QUEUE_ITEMS       (5)

/**
 * Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore!
 */

static QueueHandle_t ui_queue;
//Creates a semaphore to handle concurrent call to lvgl stuff
//If you wish to call *any* lvgl function from other threads/tasks
//you should lock on the very same semaphore!
SemaphoreHandle_t xGuiSemaphore;

#define UI_DEBUG_ENABLE
#ifndef UI_DEBUG_ENABLE
	#define UI_DEBUG(...)
#else
// Tag used for ESP serial console messages
	static const char *TAG = "[UI]";
	#define UI_LOGI(...) ESP_LOGI(TAG, LOG_COLOR(LOG_COLOR_PURPLE) __VA_ARGS__)
	#define UI_LOGE(...) ESP_LOGE(TAG, LOG_COLOR(LOG_COLOR_PURPLE) __VA_ARGS__)
	#define UI_LOGW(...) ESP_LOGW(TAG, LOG_COLOR(LOG_COLOR_PURPLE) __VA_ARGS__)
#endif


static void lv_tick_update(void *arg)
{
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}


esp_err_t ui_write_event(event_t *event)
{
    if (xQueueSend(ui_queue, event, (TickType_t)10) == pdPASS) {
        return ESP_OK;
    }
    else {
        UI_LOGE("Event Lost !!! -> [%d]\r\n", event->id);
        return ESP_FAIL;
    }
}

static esp_err_t ui_read_event(event_t *event)
{
    if (xQueueReceive(ui_queue, event, (TickType_t)10) == pdPASS)
    {
        return ESP_OK;
    }
    return ESP_FAIL;
}

static esp_err_t ui_process_event(event_t *event)
{
    switch (event->id)
    {
        case EVT_UI_SHOW_WIFI_SCREEN: ui_show_wifi_screen(); break;
        case EVT_UI_SHOW_MAIN_SCREEN: break;
        case EVT_UI_FILL_NETWORKS   : break;    
        case EVT_UI_SHOW_CONNECTING : break;  
        case EVT_UI_SHOW_SCANNING   : break;    
        case EVT_UI_SHOW_CONN_OK    : break;     
        case EVT_UI_SHOW_CONN_FAIL  : break;   

    default:
        break;
    }

    memset(event, 0, sizeof(event_t));
    return ESP_OK;
}

void ui_task(void *args) {

xGuiSemaphore = xSemaphoreCreateMutex();

    /* Initialize LVGL framework */
    lv_init();
    
    /* Initialize SPI bus used by the drivers */
    lvgl_driver_init();

    /* Define Drawing Buffers */
    static lv_color_t buf1[DISP_BUF_SIZE];
    static lv_disp_draw_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;

    #if (USE_DOUBLE_DRAW_BUFF)
        static lv_color_t buf2[DISP_BUF_SIZE];
        lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);
    #else 
        lv_disp_draw_buf_init(&disp_buf, buf1, NULL, size_in_px);
    #endif

    /* Attach Drawing Display Callbacks */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Attach Display Inputs Callbacks */
    #if USE_TOUCH_PANEL
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
    #endif

    /* Init Lv Tick timer */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_update,
        .name = "lv_tick_update"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
    
    
    ui_wifi_screen_init();

    event_t event;

    while (1) {

        vTaskDelay(1);
        //Try to lock the semaphore, if success, call lvgl stuff
        if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE) {
            lv_task_handler();   
            // Process events 
            if(ui_read_event(&event) == ESP_OK)
            {
                ui_process_event(&event);
            }
            xSemaphoreGive(xGuiSemaphore);
        }
    }

    vTaskDelete(NULL);
}


void ui_init(void)
{
    ui_queue = xQueueCreate(UI_QUEUE_ITEMS , EVENT_ITEM_SIZE_BYTES);
    xTaskCreatePinnedToCore(ui_task, "ui_task", 4096*2, NULL, 0, NULL, 1);
}
