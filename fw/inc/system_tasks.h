#ifndef _SYSTEM_TASKS_H_
#define _SYSTEM_TASKS_H_

//#include "tn_port.h"

/* Setup stack sizes and pryorities */
#define ADC_TASK_STACK_SIZE 128
#define ADC_TASK_PRIORITY 3
#define SWC_TASK_STACK_SIZE 128
#define SWC_TASK_PRIORITY 2
#define USCHED_TASK_STACK_SIZE 128
#define USCHED_TASK_PRIORITY 5
#define POWER_TASK_STACK_SIZE 64
#define POWER_TASK_PRIORITY 5
#define DBGLED_TASK_STACK_SIZE 64
#define DBGLED_TASK_PRIORITY 5
#define SWUART_TASK_STACK_SIZE 64
#define SWUART_TASK_PRIORITY 5


#undef DEBUG_SYSTEM_STACK
#ifdef DEBUG_SYSTEM_STACK
/* Collect all stacks here for debug top of stack */
extern unsigned int tn_idle_task_stack[TN_IDLE_STACK_SIZE];
extern unsigned int tn_timer_task_stack[TN_TIMER_STACK_SIZE];
extern unsigned int power_task_stack[POWER_TASK_STACK_SIZE];
extern unsigned int usched_task_stack[USCHED_TASK_STACK_SIZE];
extern unsigned int swc_task_stack[SWC_TASK_STACK_SIZE];
extern unsigned int adc_task_stack[ADC_TASK_STACK_SIZE];
#endif

#endif /* _SYSTEM_TASKS_H_ */
