#include "GXHT30.h"

static uint8_t gxht_data[GXHT_DATA_LEN]={0};

void GXHT3x_Reset(void)
{
    uint8_t buff[2]={0x30,0xA2};
    I2C_Sent(gxht_addr,buff,2);
}

void GXHT3x_LowPower(void)
{
    uint8_t buff[2]={0x24,016};
    I2C_Sent(gxht_addr,buff,2);
}

void GXHT3x_GetData(void)
{
    uint8_t buff[2]={0x2C,0x06};
    I2C_Sent(gxht_addr,buff,2);
    I2C_Read(gxht_addr,gxht_data,GXHT_DATA_LEN,100);
}

float GXHT3x_GetHum(void)
{
    float hum = 0.0;
    hum = ((((gxht_data[3] * 256.0) + gxht_data[4]) * 100) / 65535.0);
    Serial.printf("HUM:%.2f\n",hum);
    return hum;
}

float GXHT3x_GetTem(void)
{
    float tem = 0.0;
    tem = ((((gxht_data[0] * 256.0) + gxht_data[1]) * 175) / 65535.0) - 45;
    Serial.printf("TEM:%.2f\n",tem);
    return tem;
}