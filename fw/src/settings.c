#include "common.h"
#include "settings.h"
#include "stm32f10x_crc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"
#include "tinystdio.h"
#include "string.h"


#pragma pack(push)
static const struct sif_s {
	uint32_t sett_magic;
	uint32_t crc;
	settings_t settings;
} const *settings_flash = (struct sif_s *)(FLASH_OFFSET + SETTINGS_FLASH_PAGE*FLASH_PAGE_SIZE);
#pragma pack(pop)

static const settings_t settings_def = {
    .reserved1 = 0,
    .reserved2 = 0,
    .reserved3 = 0,
    .reserved4 = 0,
	.flags = 0
};

settings_t settings;
settings_status_t settings_status = SETTINGS_STATUS_DEFAULT;
bool settings_force_default = false;

uint32_t settings_getcrc(settings_t *settings);

void settings_init(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

	if (!settings_force_default && settings_flash->sett_magic == SETT_MAGIC) {
		memcpy(&settings, &settings_flash->settings, sizeof(settings_t));
		settings_status =
			(settings_flash->crc == settings_getcrc(&settings)) ?
				SETTINGS_STATUS_OK :
				SETTINGS_STATUS_INVALID_CRC;
	}
	if (settings_status != SETTINGS_STATUS_OK) {
		printf("using default settings\n\r");
		memcpy(&settings, &settings_def, sizeof(settings));
		settings_write(&settings_def);
	}
	settings_dump();
}

uint32_t settings_getcrc(settings_t *settings)
{
  CRC_ResetDR();
  return CRC_CalcBlockCRC((uint32_t *)settings, sizeof(*settings)/4 - 1);
}

static bool flash_is_page_empty(uint32_t page) {
	uint32_t i;
	for (i = 0; i < FLASH_PAGE_SIZE / sizeof(uint32_t); ++i) {
		if (*((uint32_t*)(FLASH_OFFSET + FLASH_PAGE_SIZE * page + i)) != 0xffffffff)
			return false;
	}
	return true;
}

void settings_write(const settings_t *settings) {
	uint32_t words, offset;
	
	FLASH_UnlockBank1();

	if (!flash_is_page_empty(SETTINGS_FLASH_PAGE))
	FLASH_ErasePage(SETTINGS_FLASH_OFFSET);

	words = sizeof(settings_t) / 2;
	if (words*2 < sizeof(settings_t))
		words++;
	
	for (offset = 0; offset < words; offset++) {
		FLASH_ProgramHalfWord((uint32_t)((uint16_t *)&(settings_flash->settings) + offset), ((uint16_t *)settings)[offset]);
	}

	FLASH_ProgramWord((uint32_t)&settings_flash->sett_magic, SETT_MAGIC);
	FLASH_ProgramWord((uint32_t)&settings_flash->crc, settings_getcrc((settings_t *)settings));

	FLASH_LockBank1();
}

void settings_flash_dump() {
    uint32_t i;
    DBG {
        printf("flash_dump: settings_flash magic 0x%08X crc 0x%08X\n", (unsigned int)settings_flash->sett_magic,
                (unsigned int)settings_flash->crc);
        for (i = 0; i < sizeof(settings_t); i++) {
            printf("%02x ", ((uint8_t *)&(settings_flash->settings))[i]);
        }
        printf("\n");
    }
}

void settings_dump() {
	DBG {
		printf("settings_flash magic 0x%08X crc 0x%08X\n", (unsigned int)settings_flash->sett_magic,
		        (unsigned int)settings_flash->crc);
		printf("flags: 0x%04X\r\n", settings_flash->settings.flags);
	}
}

void settings_test() {

}
