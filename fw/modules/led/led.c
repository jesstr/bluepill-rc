#include "led.h"
#include "gpio.h"
#include "common.h"
#include "FreeRTOS.h"
#include "task.h"

void led_task(void *arg) {
    (void)arg;

    while(1) {
        gpio_set(LED_GREEN, 1);
        vTaskDelay(500);
        gpio_set(LED_GREEN, 0);
        vTaskDelay(500);
    }
}