#ifndef EVENT_ROUTER_H
#define EVENT_ROUTER_H

#include "stdint.h"
#include "esp_err.h"

// #include "ui.h"
// #include "mediator.h"


#define EVENT_MAX_PAYLOAD_LEN  (32*2)

// Define macro for button events
#define BTN_EVENTS \
    EVT_BUTTON_1_PRESSED, \
    EVT_BUTTON_1_RELEASED, \
    EVT_BUTTON_2_PRESSED, \
    EVT_BUTTON_2_RELEASED, \
    EVT_BUTTON_3_PRESSED, \
    EVT_BUTTON_3_RELEASED, \
    EVT_BUTTON_4_PRESSED, \
    EVT_BUTTON_4_RELEASED 

#define BTN_EVENTS_NAME \
    "EVT_BUTTON_1_PRESSED", \
    "EVT_BUTTON_1_RELEASED", \
    "EVT_BUTTON_2_PRESSED", \
    "EVT_BUTTON_2_RELEASED", \
    "EVT_BUTTON_3_PRESSED", \
    "EVT_BUTTON_3_RELEASED", \
    "EVT_BUTTON_4_PRESSED", \
    "EVT_BUTTON_4_RELEASED" 


// Define macro for UI events
#define UI_EVENTS              \
    EVT_UI_SHOW_WIFI_SCREEN, \
    EVT_UI_SHOW_MAIN_SCREEN, \
    EVT_UI_SHOW_FOLDER_SCREEN, \
    EVT_UI_WIFI_SELECTED, \
    EVT_UI_SHARED_FOLDER_SELECTED, \
    EVT_UI_HOME_SELECTED, \
    EVT_UI_LS_BTN_CLICKED, \
    EVT_UI_SCAN_BTN_CLICKED, \
    EVT_UI_CONNECT_BTN_CLICKED, \
    EVT_UI_SET_STATUS_MSG, \
    EVT_UI_FILL_NETWORKS
  

#define UI_EVENTS_NAME \
    "EVT_UI_SHOW_WIFI_SCREEN", \
    "EVT_UI_SHOW_MAIN_SCREEN", \
    "EVT_UI_SHOW_FOLDER_SCREEN", \
    "EVT_UI_WIFI_SELECTED", \
    "EVT_UI_SHARED_FOLDER_SELECTED", \
    "EVT_UI_HOME_SELECTED", \
    "EVT_UI_LS_BTN_CLICKED", \
    "EVT_UI_SCAN_BTN_CLICKED", \
    "EVT_UI_CONNECT_BTN_CLICKED", \
    "EVT_UI_SET_STATUS_MSG", \
    "EVT_UI_FILL_NETWORKS"    

// Define macro for COIN events
#define WIFI_EVENTS \
    EVT_WIFI_STA_CONNECTED, \
    EVT_WIFI_STA_DISCONNECTED, \
    EVT_WIFI_GOT_IP

#define WIFI_EVENTS_NAME \
    "EVT_WIFI_STA_CONNECTED", \
    "EVT_WIFI_STA_DISCONNECTED",\
    "EVT_WIFI_GOT_IP"

// Define macro for COIN events
#define HTTP_EVENTS \
    EVT_HTTP_CLIENT_CONNECTED
    

#define HTTP_EVENTS_NAME \
    "EVT_HTTP_CLIENT_CONNECTED"

#define IS_VALID_EVENT(evt) (evt > EVENT_INVALID && evt < EVENT_LAST )

typedef enum
{
    TASK_INVALID,
    TASK_AIM,
    TASK_UI,
    TASK_BUTTONS,
    TASK_WIFI,
    TASK_HTTP,
    TASK_LAST, 
}task_id_t;


#define IS_VALID_TASK_ID(id) (id > TASK_INVALID && id < TASK_LAST)

typedef enum
{
   EVENT_INVALID, 
   BTN_EVENTS,
   UI_EVENTS,
   WIFI_EVENTS,
   HTTP_EVENTS,
   EVENT_LAST
}event_id_t;

typedef struct 
{
    uint32_t coin_value;
}coin_payload_t;

typedef struct
{
    uint16_t len;
    union
    {
        coin_payload_t coin;
        uint32_t net_list_msg[EVENT_MAX_PAYLOAD_LEN];
        uint32_t status_msg[EVENT_MAX_PAYLOAD_LEN];
        uint32_t buffer[EVENT_MAX_PAYLOAD_LEN];
    };
}event_payload_t;

typedef struct
{
    task_id_t  src;
    task_id_t  dest;
    event_id_t id;
    event_payload_t payload;
}event_t;



#define EVENT_ITEM_SIZE_BYTES     sizeof(event_t)

void event_router_init(void);
esp_err_t event_router_write(event_t *event);
char *event_router_get_id_name(event_id_t id);
char *event_router_get_task_name(task_id_t id);


#endif