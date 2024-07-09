
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

// User components 
#include "ui.h"
#include "event_router.h"
#include "aim.h"
#include "buttons.h"
#include "lvgl.h"

// Network interface
#include "http_server.h"
#include "wifi_app.h"
// Storage interface
#include "app_nvs.h"
#include "app_spiffs.h"

static const char *TAG = "[MAIN]";

#define PROJECT_NAME     "AIM-TH"
#define HW_VERSION       "V1R1"
#define FW_VERSION       "1.0.XXXX"
#define MCU              "ESP32-WROOM-32"

void print_startup_message(void)
{
	printf("********************************************\r\n");
	printf("  Last Compilation : %s - %s\r\n", __DATE__, __TIME__);
	printf("  Project          : %s \r\n", PROJECT_NAME);
	printf("  Hardware Version : %s\r\n", HW_VERSION);
	printf("  Firmware Version : %s\r\n", FW_VERSION);
	printf("  MCU              : %s\r\n", MCU);
    printf("  ESP-IDF version  : %d.%d.%d\n", lv_version_major(),
                                                lv_version_minor(),
                                                lv_version_patch()); 
    // using 4.4.4
    printf("  ESP-IDF version  : %d.%d.%d\n", ESP_IDF_VERSION_MAJOR,
                                                ESP_IDF_VERSION_MINOR,
                                                ESP_IDF_VERSION_PATCH); 
	printf("********************************************\r\n");
}

void init_wifi_server(void)
{
    ESP_LOGI(TAG, "Starting WiFi - HTTP Server\r\n");
    // log_for_websocket_setup();
    app_nvs_flash_setup();
    wifi_app_start();
    
    assert(app_spiffs_mount_file_system() == LOCAL_DB_SUCCESS); //> Initialize SPIFFS partition
}


void app_main(void)
{
    ui_init();
    button_init();
    init_wifi_server();
    print_startup_message();
    event_router_init();
    aim_device_init();

    ESP_LOGI(TAG, "Free memory: %d bytes", esp_get_free_heap_size());
}

