#include "cpuid.h"

#pragma pack(push)
typedef struct {
    volatile uint16_t U_ID0;
    volatile uint16_t U_ID1;
    volatile uint32_t U_ID2;
    volatile uint32_t U_ID3;
} U_ID_TypeDef;
#pragma pack(pop)

static uint32_t rnd;

uint32_t cpuid_rand(void) {
    return (rnd = rnd * 214013L + 2531011L);
}

void get_device_id(uint8_t* str) {
    ((uint16_t *)str)[0] = U_ID->U_ID0;
    ((uint16_t *)str)[1] = U_ID->U_ID1;
    ((uint16_t *)str)[2] = (uint16_t)(U_ID->U_ID2 & 0xffff);
    ((uint16_t *)str)[3] = (uint16_t)(U_ID->U_ID2 >> 16);
    ((uint16_t *)str)[4] = (uint16_t)(U_ID->U_ID3 & 0xffff);
    ((uint16_t *)str)[5] = (uint16_t)(U_ID->U_ID3 >> 16);
}
