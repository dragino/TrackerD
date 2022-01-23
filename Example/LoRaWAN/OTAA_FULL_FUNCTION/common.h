#ifndef _common_h_
#define _common_h_

#include "Arduino.h"
#include "WiFi.h"
#include "EEPROM.h"
#include "Wire.h"
#include "at.h"
#include "lora.h"
#include "GXHT30.h"
#include "LIS3DH.h"
#include "GPS.h"

#define Pro_model           "TrackerD "
#define Pro_version         "v1.0.0 "
#define EEPROM_SIZE         128
#define EEPROM_KEY_SIZE     32

#define BAT_PIN_READ    35
#define BAT_PIN_LOW     34
#define LED_PIN_RED     13
#define LED_PIN_BLUE    2
#define LED_PIN_GREEN   15

#define LBT2_SCK  5
#define LBT2_MISO 19
#define LBT2_MOSI 27
#define LBT2_SS   18

class SYS
{
  public:
    uint8_t     deveui[8]={0};   //lsb
    uint8_t     appeui[8]={0};   //lsb
    uint8_t     appkey[16]={0};  //msb
    uint8_t     appSKey[16]={0};
    uint8_t     nwkSKey[16]={0};
    uint32_t    devaddr = 0;
    uint32_t    netid;
    uint32_t    tdc=1200000;     //uint:ms

    SYS(uint8_t pin_red=LED_PIN_RED,uint8_t pin_blue=LED_PIN_BLUE,uint8_t pin_green=LED_PIN_GREEN);
    ~SYS(void);

    void config_Set(void);
    void config_Get(void);

  private:

};

class Sensor
{
  public:
    int hum;
    int tem;
    int bat;
};

extern SYS sys;
extern Sensor sensor;

int BatGet(void);
void I2C_Sent(int addr,uint8_t* buff,uint8_t buff_len);
void I2C_Read(int addr,uint8_t* buff,uint8_t buff_len,int delay_ms);
char *rtrim(char* str);
int wifi_scan(void);
void StrToHex(char *pbDest, char *pszSrc, int nLen);

#endif


  // uint8_t buff1[18]={0};
  // memcpy(buff1,WiFi.BSSID(0),6);
  // memcpy(&buff1[6],WiFi.BSSID(1),6);
  // memcpy(&buff1[12],WiFi.BSSID(2),6);

  // for(int i=0;i<18;i++)
  //   Serial.printf("%.2x ",buff1[i]);
