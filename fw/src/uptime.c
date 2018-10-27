/*
 * uptime.c
 *
 *  Created on: 19.04.2017
 *      Author: pavel
 */
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include <stdint.h>
#include "uptime.h"
#include "tinystdio.h"
#include "gpio.h"


static uint32_t Uptime = 0;


void Uptime_Init(void) {
    NVIC_InitTypeDef          NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_GetClocksFreq(&RCC_Clocks);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_SetPriority(TIM2_IRQn, 2);

    /* Set desired interrupt frequency in sec */
    uint16_t IrqFreq = 1;
    /* Set desired timer frequency in HZ */
    uint16_t TimerClk = 125;

    /* Calc desired timer load */
    uint16_t TimerLoad = TimerClk * IrqFreq;
    /* Calc desired prescaler */
    uint16_t TimerPresc = RCC_Clocks.PCLK1_Frequency / TimerClk;

    TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(TimerPresc - 1);
    TIM_TimeBaseStructure.TIM_Period = TimerLoad - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

/* Uptime timer callback */
uint32_t Uptime_GetSec(void) {
    return Uptime;
}

void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        Uptime++;
    }
}

