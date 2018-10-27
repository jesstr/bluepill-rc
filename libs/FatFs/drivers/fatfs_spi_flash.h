#ifndef FATFS_SPI_FLASH_H
    #define FATFS_SPI_FLASH+H

#include "integer.h"
#include "spi_flash.h"
int ff_spi_flash_init();
int ff_spi_flash_write(const BYTE *buff, DWORD sector, BYTE count);
int ff_spi_flash_read(const BYTE *buff, DWORD sector, BYTE count);

#endif /* FATFS_SPI_FLASH_H */
