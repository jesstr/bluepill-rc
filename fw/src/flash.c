#include "flash.h"

#define FLASH_KEY1	((uint32_t)0x45670123)
#define FLASH_KEY2	((uint32_t)0xCDEF89AB)

#define TIMEOUT_ERASE	((uint32_t)0x000B0000)
#define TIMEOUT_PROGRAM	((uint32_t)0x00002000)

static int flash_busywait(uint32_t timeout) {
	uint32_t status;
	do {
		status = FLASH->SR;
		if (status & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR))
			return -1;
		timeout--;
	} while (status & FLASH_SR_BSY && timeout);
	if (status & FLASH_SR_BSY)
		return -2;
	else
		return 0;
}

void flash_unlock(void) {
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
}

void flash_lock(void) {
	FLASH->CR |= FLASH_CR_LOCK;
}

int flash_erase_page(uint32_t address) {
	int res = 0;
	if ((res = flash_busywait(TIMEOUT_ERASE)) < 0)
		return res;
	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = address;
	FLASH->CR |= FLASH_CR_STRT;
	res = flash_busywait(TIMEOUT_ERASE);
	FLASH->CR &= ~FLASH_CR_PER;
	return res;
}

int flash_program_16bit(uint32_t address, uint16_t data) {
	int res = 0;
	if ((res = flash_busywait(TIMEOUT_PROGRAM)) < 0)
		return res;
	FLASH->CR |= FLASH_CR_PG;
	*(uint16_t *)address = data;
	res = flash_busywait(TIMEOUT_PROGRAM);
	FLASH->CR &= ~FLASH_CR_PG;
	return res;
}
