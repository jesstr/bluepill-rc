#include <stdint.h>
#include "board.h"
#include "adc.h"
#include "ma_filter.h"
#include "gpio.h"
#include "tinystdio.h"

#include "FreeRTOS.h"
#include "task.h"

#define SAMPLE_TIME     1
#define UPDATE_DELAY    50
#define VREF            3300UL

const struct{
    uint8_t chan;
    float coef;
} adc_chans[] = {
    ADC_TABLE(ADC_EXPAND_AS_DEFS)
};

/* DMA raw data */
volatile uint16_t adc_values[ADC_TOTAL_COUNT];
/* ADC values */
volatile float adc_data[ADC_TOTAL_COUNT];
/* Moving average filter */
static flt_t flt[ADC_TOTAL_COUNT];

#define MOVING_AVERAGE(chan, float_value)  moving_average(&flt[chan], float_value)

/* Channels calibration. Store to the flash if needed */
struct {
    int k;
    int b;
} adc_calib[ADC_TOTAL_COUNT] = {
    {1, 0},
    {1, 0}
};


static void update_adc_data(void)
{
    for (uint8_t i = 0; i < ADC_TOTAL_COUNT; i++) {
        float mv = (((((adc_values[i]) << 16)/4096 * VREF) >> 16) * adc_chans[i].coef) \
                * adc_calib[i].k / 1000.0 - adc_calib[i].b;
        adc_data[i] = MOVING_AVERAGE(i, mv);
    }
}


static void print_adc_data(void)
{
    printf("[adc]");
    for (uint8_t i = 0; i < ADC_TOTAL_COUNT; i++) {
        printf(" %0.3fmV", adc_data[i]);
    }
    printf("\r\n");
}


static void adc_hw_init(void)
{
    /*
    * Configure ADC to do the following:
    * - Scan through selected channels
    * - Put conversion results to memory via DMA
    * - Repeat all the thing with help of ADC CONT and DMA CIRC features
    * - While doing this, watchdog on CURRENT channel will assert interrupt when its 
    *   value goes above and below threshold
    */

    /* Configure clocks */
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);
    DMA1_Channel1->CMAR = (uint32_t)adc_values;
    DMA1_Channel1->CNDTR = ADC_TOTAL_COUNT;
    DMA1_Channel1->CCR = 
        DMA_CCR1_PL_1 |                         /* High priority */
        DMA_CCR1_MSIZE_0 | DMA_CCR1_PSIZE_0 |   /* 16-bit width */
        DMA_CCR1_MINC |                         /* Memory increment */
        DMA_CCR1_CIRC;                          /* Circular mode */
    DMA1_Channel1->CCR |= DMA_CCR1_EN;

    /* Turn ADC on */
    ADC1->CR2 |= ADC_CR2_ADON;

    ADC1->CR1 |= ADC_CR1_SCAN;
    ADC1->CR2 |= ADC_CR2_TSVREFE | ADC_CR2_DMA | ADC_CR2_CONT;

    /* Reset calibration */
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL) {
        ;
    }

    /* Start self-calibration */
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL) {
        ;
    }

    /* Set sequence length */
    ADC1->SQR1 = ((ADC_TOTAL_COUNT-1) << 20) & ADC_SQR1_L;

    /* Configure each channel and fill sequence */
    for (uint8_t i = 0; i < ADC_TOTAL_COUNT; i++) {
        uint8_t chan = adc_chans[i].chan;

        /* Init corresponding pin as analog input */
        if (chan <= 7) {
            gpio_hw_config_pin(GPIOA, chan, GPIO_MODE_IN_ANALOG);
        }
        else if (chan <= 9) {
            gpio_hw_config_pin(GPIOB, chan-8, GPIO_MODE_IN_ANALOG);
        }

        /* Add pin to sequence */
        if (i >= 12) {
            ADC1->SQR1 &= ~(0x1F << ((i - 12) * 5));
            ADC1->SQR1 |= chan << ((i - 12) * 5);
        }
        else if (i >= 6) {
            ADC1->SQR2 &= ~(0x1F << ((i - 6) * 5));
            ADC1->SQR2 |= chan << ((i - 6) * 5);
        }
        else {
            ADC1->SQR3 &= ~(0x1F << (i * 5));
            ADC1->SQR3 |= chan << (i * 5);
        }

        /* Set sample rate */
        if (chan >= 10) {
            ADC1->SMPR1 &= ~(0x07 << ((chan - 10) * 3));
            ADC1->SMPR1 |= SAMPLE_TIME << ((chan - 10) * 3);
        }
        else {
            ADC1->SMPR2 &= ~(0x07 << (chan * 3));
            ADC1->SMPR2 |= SAMPLE_TIME << (chan * 3);
        }
    }

    /* Start ADC */
    ADC1->CR2 |= ADC_CR2_ADON;
}


void adc_task(void *arg)
{
    (void)arg;
    uint32_t delay = 0;

    adc_hw_init();

    while (1) {
        update_adc_data();
        if (delay-- == 0) {
            print_adc_data();
            delay = 500 / UPDATE_DELAY;
        }
        vTaskDelay(UPDATE_DELAY);
    }
}