#ifndef BOARD_H
#define BOARD_H

#include "board_f130c8.h"

#define EXPAND_AS_ENUM(name, ...) name,
#define GPIO_EXPAND_AS_DEFS(name, port, pin, flags) {port, pin, flags},

#define ADC_EXPAND_AS_DEFS(name, chan, coef) {chan, coef},

typedef enum gpio_e {
    GPIO_TABLE(EXPAND_AS_ENUM)
    GPIO_TOTAL_COUNT
} gpio_t;

/* Prevent compilation error on pin conflict */
#define GPIO_EXPAND_AS_CHECK(name, port, pin, flags) port ## pin,
enum gpio_check {
    GPIO_TABLE(GPIO_EXPAND_AS_CHECK)
};

/* ADC channels coefficients */
#define U_COEF  1
#define I_COEF  1

#define ADC_TABLE(X) \
X( U_PS,    0,  U_COEF ) \
X( U_BAT,   1,  U_COEF )

typedef enum adc_e {
    ADC_TABLE(EXPAND_AS_ENUM)
    ADC_TOTAL_COUNT
} adc_t;


#endif /* BOARD_H */