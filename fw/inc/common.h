#ifndef COMMON_H
#define COMMON_H

#include "stm32f10x_conf.h"
#include "stm32f10x.h"

#if defined ( __GNUC__ )
#ifdef __packed
#undef __packed
#endif
#define __packed		__attribute__(( packed ))
#endif

#ifdef DEBUG
#define DBG
#else
#define DBG if (0)
#endif

// https://stackoverflow.com/questions/4415524/common-array-length-macro-for-c
#define array_len(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define _strc(x, y) x ## y
#define strc(x, y) _strc(x, y)
#define _str(x) #x
#define str(x) _str(x)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define isInterrupt() ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0)


// // define shortcuts for compiler attributes. we need to undef because modern
// // Codesourcery toolchain already have this defines.
// #ifdef __weak
// #undef __weak
// #endif
// #define __weak			__attribute__(( weak ))

// #ifdef __packed
// #undef __packed
// #endif
// #define __packed		__attribute__(( packed ))

// #ifdef __aligned
// #undef __aligned
// #endif
// #define __aligned(x)	__attribute__(( aligned(x) ))

#endif /* COMMON_H */