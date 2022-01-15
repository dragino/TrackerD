#include "LIS3DH.h"

static uint8_t LIS_data[LIS_DATA_LEN]={0};

bool LIS3DH_CON_Text(void)
{
    uint8_t buff[1]={WHO_AM_I};
    I2C_Sent(lis3dh_addr,buff,1);
    I2C_Read(lis3dh_addr,LIS_data,1,10);
    if(LIS_data[0] == 0x33)
        return 1;
    return 0;
}

void LIS3DH_Init(void)
{
    // reboot memory content, to make a clean start
    Wire.beginTransmission(lis3dh_addr);
    Wire.write(CTRL_REG5); 
    Wire.write(0x80);
    Wire.endTransmission();
 
    //set ODR = 1 Hz, normal mode, x/y/z axis enabled
    Wire.beginTransmission(lis3dh_addr);
    Wire.write(CTRL_REG1); 
    Wire.write(0x17);
    Wire.endTransmission();
 
    //set BDU= 1, scale = +/-2g, high resolution enabled
    Wire.beginTransmission(lis3dh_addr);
    Wire.write(CTRL_REG4); 
    Wire.write(0x80);
    Wire.endTransmission();

    uint8_t buff[1]={0};
    buff[0]=CTRL_REG1;buff[1]=0x17;
    I2C_Sent(lis3dh_addr,buff,2);
}

void LIS3DH_GetData(void)
{
    uint8_t buff[LIS_DATA_LEN]={OUTX_L,OUTX_H,OUTY_L,OUTY_H,OUTZ_L,OUTZ_H};
    for(uint8_t i=0;i<LIS_DATA_LEN;i++)
    {
        I2C_Sent(lis3dh_addr,&buff[LIS_DATA_LEN],1);
        I2C_Read(lis3dh_addr,&LIS_data[i],1,10);
        Serial.print(LIS_data[i],HEX);
    }    

}

