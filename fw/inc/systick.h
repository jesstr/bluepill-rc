#ifndef SYSTIMER_H
#define SYSTIMER_H

#include "common.h"

extern volatile uint64_t timer_value;

extern void systick_init(void);
extern uint32_t ms2ticks(uint32_t ms);
extern uint64_t now(void);


extern void delay_ms(uint32_t ms);
extern void delay_ms_isr(uint32_t ms);

#ifdef OS_TNKERNEL
extern void delay_us(uint32_t us);
#endif

#endif /* SYSTIMER_H */
