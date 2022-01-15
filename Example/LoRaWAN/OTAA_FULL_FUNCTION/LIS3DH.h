#ifndef _lis3dh_h_
#define _lis3dh_h_

#include "common.h"

#define         lis3dh_addr             0x19
#define         LIS_DATA_LEN            6

#define         STATUS_REG_AUX          0x07
#define         WHO_AM_I                0x0F
#define         CTRL_REG0               0x1E
#define         TEMP_CFG_REG            0x1F
#define         CTRL_REG1               0x20
#define         CTRL_REG2               0x21
#define         CTRL_REG3               0x22
#define         CTRL_REG4               0x23
#define         CTRL_REG5               0x24
#define         CTRL_REG6               0x25
#define         OUTX_L                  0x28
#define         OUTX_H                  0x29
#define         OUTY_L                  0x2A
#define         OUTY_H                  0x2B
#define         OUTZ_L                  0x2C
#define         OUTZ_H                  0x2D

bool LIS3DH_CON_Text(void);
void LIS3DH_Init(void);
void LIS3DH_GetData(void);

#endif
