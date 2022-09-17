#ifndef _gxht30_h_
#define _gxht30_h_

#include "common.h"

#define gxht_addr 0x44
#define GXHT_DATA_LEN  6


void GXHT3x_Sent(uint8_t* buff,uint8_t buff_len);
void GXHT3x_Reset(void);
void GXHT3x_LowPower(void);
void GXHT3x_GetData(void);
float GXHT3x_GetHum(void);
float GXHT3x_GetTem(void);


#endif
