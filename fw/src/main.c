#include "common.h"
#include "tinystdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "systick.h"
#include "gpio.h"
#include "uptime.h"
#include "led.h"
#include "usart.h"


uint8_t ucHeap[configTOTAL_HEAP_SIZE];
uint32_t crc __attribute__ ((section (".crc"))) = 0xAABBCCDD;


/* OS stack overflow hook */
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
    volatile signed char *name;
    volatile xTaskHandle *pxT;

    name = pcTaskName;
    pxT = pxTask;
    (void)pxT;

    DBG printf("vApplicationStackOverflowHook\r\n");
    DBG printf("pcTaskName: %s\r\n", name);

    while(1);
}
#endif


/* OS malloc failed hook */
#if (configUSE_MALLOC_FAILED_HOOK > 0)
void vApplicationMallocFailedHook(void)
{
    DBG printf("vApplicationMallocFailedHook\r\n");

    while(1);
}
#endif


/* Init task, it runs all other tasks */
void InitTask(void *arg)
{
    (void)arg;

    Uptime_Init();

    /* Led blink task */
    xTaskCreate(led_task, "ledtask", configMINIMAL_STACK_SIZE, \
                NULL, tskIDLE_PRIORITY, NULL);

    /* Successful boot */
    printf("MCU started at %u MHz\r\n", (unsigned int)SystemCoreClock);

    gpio_set(LED_GREEN, true);

    vTaskDelete(NULL);
    taskYIELD();
}


int main(void)
{
    __disable_irq();
    NVIC_SetPriorityGrouping(0);
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
    __enable_irq();

    gpio_init();
    uart_init();

    xTaskCreate( InitTask, "InitTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    vTaskStartScheduler();

    while(true);
    return 0;
}
