#include "fatfs_spi_flash.h"
#include "spi_flash.h"
#include "integer.h"

int ff_spi_flash_init() {
    int res = 0;
    res = spi_flash_init();
    return res;
}
int ff_spi_flash_write(const BYTE *buff, DWORD sector, BYTE count) {
    int i, res = 0, ret = 0, offset = 0;
    for (i = 1; i<=count; i++) {
	spi_flash_erase_sector(sector*SPI_FLASH_SECTOR_SIZE*i, 0);
    }
    res = spi_flash_write(sector * SPI_FLASH_SECTOR_SIZE, buff, count * SPI_FLASH_SECTOR_SIZE, 0);
    return res;
}
int ff_spi_flash_read(const BYTE *buff, DWORD sector, BYTE count) {
    spi_flash_read(sector * SPI_FLASH_SECTOR_SIZE, buff, count * SPI_FLASH_SECTOR_SIZE, 0);
    return 0;
}

