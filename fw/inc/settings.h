#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include "board.h"


#pragma pack(push)
typedef struct {
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t reserved4;
	uint16_t flags;
} settings_t;
#pragma pack(pop)

typedef enum settings_status_e {
	SETTINGS_STATUS_DEFAULT = 0,
	SETTINGS_STATUS_OK,
	SETTINGS_STATUS_INVALID_CRC
} settings_status_t;

#define SETT_MAGIC				0x95689568

#define FLASH_OFFSET			0x08000000

/* high-density value line devices */
#define FLASH_PAGE_SIZE         1024
#define SETTINGS_FLASH_PAGE		63
#define SETTINGS_FLASH_SIZE		FLASH_PAGE_SIZE
#define SETTINGS_FLASH_OFFSET	(FLASH_OFFSET + FLASH_PAGE_SIZE*SETTINGS_FLASH_PAGE)

extern settings_t settings;
extern settings_status_t settings_status;
extern bool settings_force_default;

extern void settings_init(void);
extern void settings_write(const settings_t *settings);
void settings_flash_dump();
extern void settings_dump();

#endif /* SETTINGS_H */
