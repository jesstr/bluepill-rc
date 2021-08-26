
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "led.h"
#include "gpio.h"
#include "common.h"

#include "FreeRTOS.h"
#include "task.h"

extern volatile uint8_t Receive_Buffer[64];
extern volatile uint32_t Receive_length ;
extern volatile uint32_t length ;
uint8_t Send_Buffer[64];
uint32_t packet_sent=1;
uint32_t packet_receive=1;


void usb_task(void *arg) {
    (void)arg;

    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();

    while(1) {
        if (bDeviceState == CONFIGURED) {
            CDC_Receive_DATA();
            // Check to see if we have data yet
            if (Receive_length != 0) {
                // If received symbol '1' then LED turn on, else LED turn off
                if (Receive_Buffer[0]=='1') {
                    gpio_set(LED_GREEN, 1);
                } else {
                    gpio_set(LED_GREEN, 0);
                }
                // Echo
                if (packet_sent == 1) {
                    CDC_Send_DATA((uint8_t*)Receive_Buffer, Receive_length);
                }
                Receive_length = 0;
            }
        }
        vTaskDelay(100);
    }
}