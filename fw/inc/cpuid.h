#ifndef CPUID_H
#define CPUID_H

#include "common.h"

#define U_ID_BASE   ((uint8_t *)0x1FFFF7E8)
#define U_ID_LEN    12
#define U_ID        ((U_ID_TypeDef *) U_ID_BASE)

uint32_t cpuid_rand(void);
void get_device_id(uint8_t* str);

#endif /* CPUID_H */
