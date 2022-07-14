#ifndef _GPS_h_
#define _GPS_h_

#include "common.h"

#define GPS_POWER 12

void GPS_Init(uint8_t power_pin=GPS_POWER);
void GPS_DeInit(void);
void GPS_boot(void);
void GPS_shutdown(void);
bool GPS_DATA(void);

#endif
