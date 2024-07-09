#include "toggleLed.h"

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"

#define LED    (2)

void init_led(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

void toggle_led(bool is_on)
{
    gpio_set_level(LED, is_on);
}
