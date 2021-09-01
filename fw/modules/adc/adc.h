#ifndef ADC_H
#define ADC_H

#include "common.h"
#include "board.h"

typedef struct {
    uint8_t chan;
    uint32_t flags;
} adc_chandef_t;

#define MCU_TEMP_CHAN	16
#define MCU_VREF_CHAN	17

void adc_task(void *arg);


#endif /* ADC_H */