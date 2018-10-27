#include "common.h"
#include "hal.h"
#include "gpio.h"
#include "usart.h"

void hal_init() {
	SystemInit();
	NVIC_SetPriorityGrouping(0);
	uart_init();
}