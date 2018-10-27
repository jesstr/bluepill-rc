#ifndef FLASH_H
#define FLASH_H

#include "common.h"

#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
#define FLASH_PAGE_SIZE		((uint16_t)0x800)
#else
#define FLASH_PAGE_SIZE		((uint16_t)0x400)
#endif

extern void flash_unlock(void);
extern void flash_lock(void);
extern int flash_program_16bit(uint32_t address, uint16_t data);
extern int flash_erase_page(uint32_t address);

#endif /* FLASH_H */
