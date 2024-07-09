#ifndef AIM_DEVICE_H
#define AIM_DEVICE_H

#include "event_router.h"
#include "esp_err.h"

typedef enum
{
    STATE_INVALID = 0, 
    STATE_WIFI_CONFIG, 
    STATE_MAIN_SCREEN, 
    STATE_SHARED_FOLDER, 
    STATE_LAST
}aim_device_state_t;

typedef struct
{

}aim_device_iface_t;

typedef struct
{
    aim_device_state_t state;
    aim_device_iface_t iface;
    event_t event;
}aim_device_fsm_t;

void aim_device_init(void);
esp_err_t aim_device_write_event(event_t *event);


#endif 