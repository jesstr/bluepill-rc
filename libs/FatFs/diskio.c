/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "stm32f4xx.h"
#include "ffconf.h"

/*
 * temporarily
 */
#define FATFS_SPI_FLASH

#ifdef FATFS_SDCARD
    #include "sdcard.h"
#endif

#ifdef FATFS_SPI_FLASH
    #include "drivers/fatfs_spi_flash.h"
#endif


/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
/*-----------------------------------------------------------------------*/

#define SECTOR_SIZE 512

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
) {
    int res;
    switch (drv) {
    case SPI_FLASH:
        res = ff_spi_flash_init();
    }
#ifdef FATFS_SDCARD
  NVIC_InitTypeDef NVIC_InitStructure;
  SD_Error res = SD_OK;

  /* Configure the NVIC Preemption Priority Bits */
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

  res =  SD_Init(); 

  if(res == SD_OK)
  {
    res = (SD_Error)0x0;
  }
  return ((DSTATUS)res);
#endif

}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
  if (drv) return STA_NOINIT;		/* Supports only single drive */
  return 0;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
    int res;
#ifdef FATFS_SPI_FLASH
    res = ff_spi_flash_read(buff, sector,  count);
#endif

#ifdef FATFS_SDCARD
  SD_Error Status = SD_OK;
  
  for (int secNum = 0; (secNum < count) && (Status == SD_OK); secNum++) {
    Status = SD_ReadBlock((buff+SECTOR_SIZE*secNum), (sector+secNum)*SECTOR_SIZE, SECTOR_SIZE);
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }
  
/*  
  SD_Error status = SD_OK;

  SD_ReadMultiBlocks(buff, sector << 9, 512, 1);

  // Check if the Transfer is finished 
  status =  SD_WaitReadOperation();
  while (SD_GetStatus() != SD_TRANSFER_OK);

  if (status == SD_OK) {
    return RES_OK;
  } else {
    return RES_ERROR;
  }
*/
#endif /* FATFS_SDCARD */
  return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
/* The FatFs module will issue multiple sector transfer request
/  (count > 1) to the disk I/O layer. The disk function should process
/  the multiple sector transfer properly Do. not translate it into
/  multiple single sector transfers to the media, or the data read/write
/  performance may be drasticaly decreased. */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
    int res;
#ifdef FATFS_SPI_FLASH
    res = ff_spi_flash_write(buff, sector, count);
#endif

#ifdef FATFS_SDCARD
  SD_Error Status = SD_OK;
  
  for (int secNum = 0; (secNum < count) && (Status == SD_OK); secNum++) {
  Status = SD_WriteBlock((uint8_t *) buff+SECTOR_SIZE*secNum, (sector+secNum)*SECTOR_SIZE, SECTOR_SIZE);
  while(SD_GetStatus() != SD_TRANSFER_OK);
  }
/*  
  SD_Error status = SD_OK;

  SD_WriteMultiBlocks((BYTE *)buff, sector << 9, 512, 1);

  // Check if the Transfer is finished 
  status = SD_WaitWriteOperation();
  while(SD_GetStatus() != SD_TRANSFER_OK);     
  if (status == SD_OK) {
    return RES_OK;
  } else {
    return RES_ERROR;
  }
*/  

#endif /* FATFS_SDCARD */
  return res;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Get current time                                                      */
/*-----------------------------------------------------------------------*/

DWORD get_fattime ()
{
  return ((2006UL-1980) << 25)	      // Year = 2006
          | (2UL << 21)	      // Month = Feb
          | (9UL << 16)	      // Day = 9
          | (22U << 11)	      // Hour = 22
          | (30U << 5)	      // Min = 30
          | (0U >> 1)	      // Sec = 0
          ;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT res = RES_OK;
  switch (ctrl) {
	

    case GET_SECTOR_COUNT :	  // Get number of sectors on the disk (DWORD)
//      *(DWORD*)buff = 131072;	// 4*1024*32 = 131072
	*(DWORD*)buff = SPI_FLASH_BLOCK_SIZE * SPI_FLASH_BLOCK_NUMBER;
      res = RES_OK;
      break;

    case GET_SECTOR_SIZE :	  // Get R/W sector size (WORD) 
//      *(WORD*)buff = 512;
	*(WORD*)buff = SPI_FLASH_SECTOR_SIZE;
   
      res = RES_OK;
      break;

    case GET_BLOCK_SIZE :	    // Get erase block size in unit of sector (DWORD)
//      *(DWORD*)buff = 32;
      *(DWORD*)buff = SPI_FLASH_SECTORS_IN_BLOCK_NUMBER;
      res = RES_OK;
  }
	  
  return res;
}
