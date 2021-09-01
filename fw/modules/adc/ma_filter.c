#include "ma_filter.h" 


float moving_average(flt_t *flt, float new_value)
{
    if (!(flt->head < FILTER_LEN))
        flt->head = 0;

    if (flt->count < FILTER_LEN)
        flt->count++;

    flt->sum -= flt->buf[flt->head];
    flt->sum += new_value;
    flt->buf[flt->head++] = new_value;

    return flt->sum / flt->count;
}