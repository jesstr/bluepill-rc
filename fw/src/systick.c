#include "common.h"
#include "systick.h"
#include "gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#define SYSTICK_PERIOD 1000ul		// 1ms

volatile uint64_t timer_value;


uint64_t now() {
	return xTaskGetTickCountFromISR();
}

#if 0
uint32_t ms2ticks(uint32_t ms) {
	uint32_t res, us;
	us = ms*1000UL;
	if (ms == 0xFFFFFFFF)
		return TN_WAIT_INFINITE;
	if (us < SYSTICK_PERIOD && ms > 0)
		return 1UL;
	res = us / SYSTICK_PERIOD;
	if ((us % SYSTICK_PERIOD) << 1 >= SYSTICK_PERIOD)
		++res;
	return res;
}
#endif

void delay_ms_isr(uint32_t ms) {
	uint64_t stop = xTaskGetTickCountFromISR() + ms;
	while (xTaskGetTickCountFromISR() < stop);
}

void delay_ms(uint32_t ms) {
	uint64_t stop = xTaskGetTickCount() + ms;
	while (xTaskGetTickCount() < stop);
}