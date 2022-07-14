#include "common.h"

SYS sys;
Sensor sensor;
Devicet devicet;

//#define buzzer (4)
int freq = 2000;//设置频率
int channel = 0;//通道号，取值0 ~ 15
int resolution = 8;//计数位数，2的8次幂=256

//// Instantiate eeprom objects with parameter/argument names and sizes
EEPROMClass  KEY("eeprom0");
EEPROMClass  DATA("eeprom1");
EEPROMClass  GPSDATA("eeprom2");

SYS::SYS(uint8_t pin_red,uint8_t pin_blue,uint8_t pin_green)
{
  pinMode(pin_red,OUTPUT);
  pinMode(pin_blue,OUTPUT);
  pinMode(pin_green,OUTPUT);
  pinMode(BAT_PIN_READ,INPUT);
  pinMode(BAT_PIN_LOW,OUTPUT);
//  digitalWrite(BAT_PIN_LOW, HIGH);
}

SYS::~SYS(void)
{
  
}

int BatGet(void)
{
  int   bat_i=0;
  float bat_f = 3.3/4095*analogRead(BAT_PIN_READ);
  Serial.printf("BAT:%.2f V\r\n",bat_f);
  bat_i = (int)(bat_f*1000);
  return bat_i;
}

void buzzer(void)
{
  pinMode(BAT_PIN_LOW,OUTPUT);
//  digitalWrite(BAT_PIN_LOW, LOW);
//  delay(1000);
  digitalWrite(BAT_PIN_LOW, HIGH);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(BAT_PIN_LOW, channel);  
  ledcWriteTone(channel, 2000);
  for (int dutyCycle = 255; dutyCycle <= 255; dutyCycle = dutyCycle + 10) {
//    Serial.println(dutyCycle);
    ledcWrite(channel, dutyCycle);
    delay(500);
  }  
  ledcWrite(channel, 125);
}

void Stop_buzzer(void)
{
  pinMode(BAT_PIN_LOW,OUTPUT);
  freq = 0;
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(BAT_PIN_LOW, channel);  
  ledcWriteTone(channel, 2000);
  for (int dutyCycle = 0; dutyCycle <= 0; dutyCycle = dutyCycle ) {
//    Serial.println(dutyCycle);
    ledcWrite(channel, dutyCycle);
    delay(500);
    break;
  }  
}

void Device_status()
{
  uint8_t freq_band;
  uint16_t version;
  
  devicet.battrey = BatGet();
  if(sys.sensor_mode==1)
  { 
    devicet.sensor_type = 0x0A;
  }
  else if(sys.sensor_mode==2)
  {
    devicet.sensor_type = 0x0E;
  }
  sys.fire_version = string_touint();
  if(sys.fire_version>100)
  {
    version=(sys.fire_version/100)<<8|(sys.fire_version/10)<<4|(sys.fire_version%10);
  }
  else
  {
    version=(sys.fire_version/10)<<8|(sys.fire_version%10)<<4;    
  }
  devicet.firm_ver = version;

  #if defined(CFG_eu868)
    freq_band=0x01;
  #elif defined( CFG_us915)
    freq_band=0x02;
  #elif defined( CFG_in866 )
    freq_band=0x03;
  #elif defined( CFG_au915 )
    freq_band=0x04;
  #elif defined( CFG_KZ865 )
    freq_band=0x05;
  #elif defined( CFG_RU864 )
    freq_band=0x06;
  #elif defined( CFG_as923 )
    #if defined AS923_1
    freq_band=0x08;
    #elif defined AS923_2
    freq_band=0x09;
    #elif defined AS923_3
    freq_band=0x0A;
    #else
    freq_band=0x07;
    #endif
  #elif defined( REGION_CN470 )
    freq_band=0x0B;
  #elif defined( REGION_EU433 )
    freq_band=0x0C;
  #elif defined( CFG_kr920 ) 
    freq_band=0x0D;
  #elif defined( REGION_MA869 ) 
    freq_band=0x0E;
  #else
    freq_band=0x00;
  #endif
  devicet.freq_band = freq_band;
  
  #if defined( REGION_us915 ) || defined( REGION_au915 ) || defined( REGION_CN470 )
  devicet.sub_band = sys.channel_single;
  #else
  devicet.sub_band = 0xff;
  #endif
}

uint16_t string_touint(void)
{
  char *p;  
  uint8_t chanum=0; 
  uint16_t versi;
  char version[8]="";
  p=Pro_version;
  
  while(*p++!='\0')
  {
  if(*p>='0'&&*p<='9')
   {
     version[chanum++]=*p;
   }     
  }
  versi=atoi(version);
  
  return versi;
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


void SYS::eeprom_init(void)
{
  if (!KEY.begin(EEPROM_KEY_SIZE)) {
    Serial.println("Failed to initialise NAMES");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!DATA.begin(EEPROM_DATA_SIZE)) {
    Serial.println("Failed to initialise HEIGHT");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!GPSDATA.begin(EEPROM_GPSDATA_SIZE)) {
    Serial.println("Failed to initialise AGE");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }  
}

/**************       Read and write and storage     **************/

void SYS::config_Write(void)
{
  int       addr = 0;
  int       addr1 = 0;
  uint8_t   buff[16]={0};
  uint32_t  data_32;
  uint8_t   data_8;
//deveui
  LORA_GetDEUI(buff);
  for(uint8_t i=0;i<8;i++)
  {
    KEY.writeUChar(addr, buff[i]);               
    addr += sizeof(unsigned char);
  }
//appeui
  LORA_GetAEUI(buff);
  for(uint8_t i=0;i<8;i++)
  {
    KEY.writeUChar(addr, buff[i]);               
    addr += sizeof(unsigned char);
  }
//appkey
  LORA_GetAKEY(buff);
  for(uint8_t i=0;i<16;i++)
  {
    KEY.writeUChar(addr, buff[i]);               
    addr += sizeof(unsigned char);
  }
//appSKey
  LORA_GetAPPSKEY(buff);
  for(uint8_t i=0;i<16;i++)
  {
    KEY.writeUChar(addr, buff[i]);               
    addr += sizeof(unsigned char);
  }
//nwkSKey
  LORA_GetNWKSKEY(buff);
  for(uint8_t i=0;i<16;i++)
  {
    KEY.writeUChar(addr, buff[i]);               
    addr += sizeof(unsigned char);
  }
//devaddr
  data_32 = LORA_GetDevaddr();
  KEY.writeUInt(addr, data_32);           
  addr += sizeof(unsigned int);
//tdc
  DATA.writeUInt(addr1, tdc);               
  addr1 += sizeof(unsigned int);
//adr_flag
  data_8 = LORA_GetADR();
  DATA.writeByte(addr1, data_8);               
  addr1 += sizeof(byte);
//dr
  data_8 = LORA_GetDR();
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);
//txp
  data_8 = LORA_GetTXP();
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);
//mod
  data_8 = mod;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);
//lon
  data_8 = lon;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  data_8 = alarm;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);
  
  data_8 = gps_start;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  data_8 = gps_alarm;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  data_8 = alarm_count;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  data_8 = port;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  data_8 = sensor_mode;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  data_8 = channel_single;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char); 

  DATA.writeUInt(addr1, Positioning_time);               
  addr1 += sizeof(unsigned int);

  DATA.writeUInt(addr1, addr_gps_write);               
  addr1 += sizeof(unsigned int);    

  DATA.writeUInt(addr1, addr_gps_read);               
  addr1 += sizeof(unsigned int); 
    
  data_8 = loggpsdata_send;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);  

  data_8 = frame_flag;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);    
        
  KEY.commit();
  DATA.commit();
  addr = 0;
}

void SYS::config_Read(void)
{ 
  int       addr = 0;
  int       addr1 = 0;  
  uint8_t   buff[16]={0};
  uint32_t  data_32;
  uint32_t  data_8;
//deveui
  for(uint8_t i=0;i<8;i++)
  {
    buff[i] = KEY.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  LORA_SetDEUI(buff);
//appeui
  for(uint8_t i=0;i<8;i++)
  {
    buff[i] = KEY.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  LORA_SetAEUI(buff);
//appkey
  for(uint8_t i=0;i<16;i++)
  {
    buff[i] = KEY.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  LORA_SetAKEY(buff);
//appSKey
  for(uint8_t i=0;i<16;i++)
  {
    buff[i] = KEY.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  LORA_SetAPPSKEY(buff);
//nwkSKey
  for(uint8_t i=0;i<16;i++)
  {
    buff[i] = KEY.readUChar(addr);
    addr += sizeof(unsigned char);
  }
  LORA_SetNWKSKEY(buff);
//devaddr 
  data_32 = KEY.readUInt(addr);
  LORA_SetDevaddr(data_32);
  addr += sizeof(unsigned int);
//tdc
  tdc = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);
  if(tdc == 0)
    tdc = 1200000;
//adr_flag
  data_8 = DATA.readByte(addr1);      
  LORA_SetADR(data_8);         
  addr1 += sizeof(byte);
//dr
  data_8 = DATA.readUChar(addr1);            
  LORA_SetDR(data_8);        
  addr1 += sizeof(unsigned char);
//txp
  data_8 = DATA.readUChar(addr1);  
  LORA_SetTXP(data_8);                  
  addr1 += sizeof(unsigned char);
//mod
  mod = DATA.readUChar(addr1);                   
  addr1 += sizeof(unsigned char);  
//lon
  lon = DATA.readUChar(addr1);                   
  addr1 += sizeof(unsigned char);  
  if(lon == 0)
    lon = 1;
  alarm = DATA.readUChar(addr1);                   
  addr1 += sizeof(unsigned char);  
//  if(alarm == 0)
//    alarm = 1;     
  gps_start = DATA.readUChar(addr1);                   
  addr1 += sizeof(unsigned char);  
//  if(gps_start == 0)
//    gps_start = 1; 
  gps_alarm = DATA.readUChar(addr1);                   
  addr1 += sizeof(unsigned char);  
//  if(gps_alarm == 0)
//    gps_alarm = 1;    
  alarm_count = DATA.readUChar(addr1);                   
  addr1 += sizeof(unsigned char);   

  port = DATA.readUChar(addr1);                   
  addr1 += sizeof(unsigned char);  

  sensor_mode = DATA.readUChar(addr1);                   
  addr1 += sizeof(unsigned char);  

  channel_single = DATA.readUChar(addr1);                   
  addr1 += sizeof(unsigned char);  

  Positioning_time = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);
  if(Positioning_time == 0)
    Positioning_time = 180000;  

  addr_gps_write = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);

  addr_gps_read = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int); 
  if(addr_gps_read <=0)
      addr_gps_read = 0;

  loggpsdata_send = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned int);    
  
  frame_flag = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned int); 
         
}

void SYS::gps_data_Weite(void)
{
  for(uint8_t i=0;i<15;i++)
  {
    GPSDATA.writeUChar(addr_gps_write, sys.gps_data_buff[i]);               
    addr_gps_write += sizeof(unsigned char);
  }         
  sys.config_Write();
  GPSDATA.commit();   
}

void SYS::gps_data_Read(void)
{
  for(uint8_t i=0;i<15;i++)
  {
    sys.gps_data_buff[i] = GPSDATA.readUChar(addr_gps_read);
    addr_gps_read += sizeof(unsigned char);
  }
}

void SYS::read_gps_data_on_flash()
{
  gps_data_Read();
  Serial.print("GPS DATA:"); 
  for(int i=0;i<15;i++)
      Serial.printf("%.2x ",sys.gps_data_buff[i]);
  Serial.println();
}

void SYS::GPSDATA_CLEAR(void)
{
  int addr = 0;
  for(int i=addr;i<addr_gps_write;i++)
  {
    GPSDATA.writeByte(addr, 0);
    addr +=sizeof(byte);     
  }
  GPSDATA.commit();
  addr = 0;
}
void SYS::DATA_CLEAR(void)
{
  int addr = 0;
  for(int i=addr;i<EEPROM_DATA_SIZE;i++)
  {
    DATA.writeByte(addr, 0);
    addr +=sizeof(byte);     
  }
  DATA.commit();
  addr = 0;
}
void StrToHex(uint8_t *pbDest, char *pszSrc, int nLen)
{
	uint8_t h1, h2;
	uint8_t s1, s2;
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
