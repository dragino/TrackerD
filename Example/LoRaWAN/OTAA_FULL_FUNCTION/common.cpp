#include "common.h"

SYS sys;
Sensor sensor;

SYS::SYS(uint8_t pin_red,uint8_t pin_blue,uint8_t pin_green)
{
  pinMode(pin_red,OUTPUT);
  pinMode(pin_blue,OUTPUT);
  pinMode(pin_green,OUTPUT);
  pinMode(BAT_PIN_READ,INPUT);
  pinMode(BAT_PIN_LOW,OUTPUT);
  digitalWrite(BAT_PIN_LOW, LOW);
}

SYS::~SYS(void)
{
  
}

int BatGet(void)
{
  int   bat_i;
  float bat_f = 3.3/4095*analogRead(BAT_PIN_READ);
  Serial.printf("BAT:%.2f mV\r\n",bat_f);
  bat_i = (int)(bat_f*1000);
  return bat_i;
}

void I2C_Sent(int addr,uint8_t* buff,uint8_t buff_len)
{
  Wire.beginTransmission(addr);
  for(uint8_t i=0;i<buff_len;i++)
  {
     Wire.write(buff[i]);
  }
  Wire.endTransmission();
}

void I2C_Read(int addr,uint8_t* buff,uint8_t buff_len,int delay_ms)
{
  memset(buff,0,buff_len);
  delay(delay_ms);
  Wire.requestFrom(addr, buff_len);
  int len = Wire.available();
  if (len != 0)
  {
    for(uint8_t i=0;i<len;i++)
    {
       buff[i] = Wire.read();
    } 
  }
}

char *rtrim(char* str)
{
  for(int i=0;i<strlen(str);i++)
  {
    if(str[i]=='\r' || str[i]=='\n')
      str[i] = 0;
  } 
  return str;
}

int wifi_scan(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Scanning...");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) 
  {
    Serial.println("no networks found");
  } 
  else 
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) 
    {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      Serial.print(" ");
      Serial.println(WiFi.BSSIDstr(i));
    }
  }
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  return n;
}

/**************       Read and write and storage     **************/

void SYS::config_Set(void)
{
  int addr = 0;

  for(uint8_t i=0;i<8;i++)
  {
    EEPROM.writeUChar(addr, deveui[i]);               
    addr += sizeof(unsigned char);
  }
  for(uint8_t i=0;i<8;i++)
  {
    EEPROM.writeUChar(addr, appeui[i]);               
    addr += sizeof(unsigned char);
  }
  for(uint8_t i=0;i<16;i++)
  {
    EEPROM.writeUChar(addr, appkey[i]);               
    addr += sizeof(unsigned char);
  }
  for(uint8_t i=0;i<16;i++)
  {
    EEPROM.writeUChar(addr, appSKey[i]);               
    addr += sizeof(unsigned char);
  }
  for(uint8_t i=0;i<16;i++)
  {
    EEPROM.writeUChar(addr, nwkSKey[i]);               
    addr += sizeof(unsigned char);
  }

  EEPROM.writeUInt(addr, devaddr);           
  addr += sizeof(unsigned int);

  EEPROM.writeUInt(addr, tdc);               
  addr += sizeof(unsigned int);


  EEPROM.commit();
  addr = 0;
}

void SYS::config_Get(void)
{ 
  int addr = 0;
  for(uint8_t i=0;i<8;i++)
  {
    deveui[i] = EEPROM.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  for(uint8_t i=0;i<8;i++)
  {
    appeui[i] = EEPROM.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  for(uint8_t i=0;i<16;i++)
  {
    appkey[i] = EEPROM.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  for(uint8_t i=0;i<16;i++)
  {
    appSKey[i] = EEPROM.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  for(uint8_t i=0;i<16;i++)
  {
    nwkSKey[i] = EEPROM.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  
  devaddr = EEPROM.readUInt(addr);
  addr += sizeof(unsigned int);

  tdc = EEPROM.readUInt(addr);
  addr += sizeof(unsigned int);
  if(tdc == 0)
    tdc = 1200000;
  
  
}

void StrToHex(char *pbDest, char *pszSrc, int nLen)
{
	char h1, h2;
	char s1, s2;
	for (int i = 0; i < nLen; i++)
	{
		h1 = pszSrc[2 * i];
		h2 = pszSrc[2 * i + 1];
 
		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
			s1 -= 7;
 
		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
			s2 -= 7;
 
		pbDest[i] = s1 * 16 + s2;
	}
}

