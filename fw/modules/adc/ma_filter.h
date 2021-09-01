#ifndef MA_FILTER_H
#define MA_FILTER_H

#include <stdint.h>

#define FILTER_LEN 20

typedef struct {
    float sum;
    uint8_t head;
    uint8_t count;
    float buf[FILTER_LEN];
} flt_t;

float moving_average(flt_t *flt, float new_value);



#endif /* MA_FILTER_H */