#ifndef VALUES_H
#define VALUES_H

#include <stdint.h>

#define FILTER_LEN 20
typedef struct {
	float sum;
	uint8_t head;
	uint8_t count;
	float buf[FILTER_LEN];
} flt_t;

float moving_average(flt_t *flt, float new_value);
	
#define _bswap32(x) 							\
	((((x) & 0xff000000UL) >> 24) | (((x) & 0x00ff0000UL) >>  8) |	\
	(((x) & 0x0000ff00UL) <<  8) | (((x) & 0x000000ffUL) << 24))

typedef union {
	int i;
	float f;
} int_float_u;

volatile int bswap32(int n);
unsigned int CPU_swap_u32(unsigned int value);

#define abs(x) if((x) < 0) \
			(x) *= -1;

	       	
			

#endif /* VALUES_H */
