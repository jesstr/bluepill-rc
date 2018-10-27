//      crc8.c
//      
//      Copyright 2011 Dmitry Repnikov <drepnikov@switron.ru>
//      

#include <stdint.h>
#include "crc8.h"

uint8_t calc_crc8_pkt(uint8_t command, uint8_t frame, uint8_t *pcBlock, uint8_t len) {
	uint8_t crc = 0xFF;

	crc = Crc8Table[crc ^ command];
	crc = Crc8Table[crc ^ frame];
	crc = Crc8Table[crc ^ len];
	while (len--) crc = Crc8Table[crc ^ *pcBlock++];
	return crc;
}

uint8_t calc_crc8_dta(uint8_t *pcBlock, uint8_t len) {
	if (len < 3) return 0x00;
	return calc_crc8_pkt( pcBlock[0], pcBlock[1], &pcBlock[3], pcBlock[2] );
}


