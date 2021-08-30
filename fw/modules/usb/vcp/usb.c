
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "led.h"
#include "gpio.h"
#include "common.h"

#include "FreeRTOS.h"
#include "task.h"



void usb_task(void *arg) {
    (void)arg;
    uint32_t len;
    static uint8_t buf[VIRTUAL_COM_PORT_DATA_SIZE];

    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
    CDC_Init();

    while (1) {
        if (bDeviceState == CONFIGURED) {
            if ((len = CDC_Receive_DATA(buf, portMAX_DELAY)) > 0) {
                // If received symbol '1' then LED turn on, else LED turn off
                if (buf[0]=='1') {
                    gpio_set(LED_GREEN, 1);
                } else {
                    gpio_set(LED_GREEN, 0);
                }
                /* Echo */
                CDC_Send_DATA(buf, len);
            }
        }
    }
}