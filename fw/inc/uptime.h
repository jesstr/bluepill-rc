/*
 * uptime.h
 *
 *  Created on: 19.04.2017
 *      Author: pavel
 */

#ifndef UPTIME_H_
#define UPTIME_H_

#include <stdint.h>

void Uptime_Init(void);
uint32_t Uptime_GetSec(void);

#endif /* UPTIME_H_ */
