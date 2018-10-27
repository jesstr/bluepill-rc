#include "tim.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"


void StatTimer_Init(void)
{
  //NVIC_InitTypeDef          NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);

  /*
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 2);
  */

  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(SystemCoreClock / 1000) - 1;;
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

  TIM_Cmd(TIM12, ENABLE);
  //TIM_ITConfig(TIM12, TIM_IT_Update, ENABLE);
}

uint32_t GetStatTimerValue(void) {
    return TIM_GetCounter(TIM12);
}



void TIM8_UP_TIM12_IRQHandler(void)
{

}

