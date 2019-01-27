#include "common.h"

#include "FreeRTOS.h"
#include "task.h"

#include "tinystdio.h"


/* OS stack overflow hook */
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
    volatile signed char *name;
    volatile xTaskHandle *pxT;

    name = pcTaskName;
    pxT = pxTask;
    (void)pxT;

    printf("vApplicationStackOverflowHook\r\n");
    printf("pcTaskName: %s\r\n", name);

    __disable_irq();
    NVIC_SystemReset();
    while(1);
}
#endif

/* OS malloc failed hook */
#if (configUSE_MALLOC_FAILED_HOOK > 0)
void vApplicationMallocFailedHook(void)
{
    printf("vApplicationMallocFailedHook\r\n");

    __disable_irq();
    NVIC_SystemReset();
    while(1);
}
#endif

/* Force Hardfault for debug purpose */
void Force_HardFault(void) {
    void (*foo)(void) = NULL;
    foo();
}