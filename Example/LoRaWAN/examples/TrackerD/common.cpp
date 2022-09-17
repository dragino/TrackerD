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
  float bat_f = 3.46/4095*analogRead(BAT_PIN_READ);
  bat_f = bat_f*570/470;
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
    Serial.println("Failed to initialise KEY");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!DATA.begin(EEPROM_DATA_SIZE)) {
    Serial.println("Failed to initialise DATA");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  if (!GPSDATA.begin(EEPROM_GPSDATA_SIZE)) {
    Serial.println("Failed to initialise GPSDATA");
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
//mtdc
  DATA.writeUInt(addr1, mtdc);               
  addr1 += sizeof(unsigned int);  
//atdc
  DATA.writeUInt(addr1, atdc);               
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

  DATA.writeUInt(addr1, fre);               
  addr1 += sizeof(unsigned int);   
    
  data_8 = frame_flag;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);  

  data_8 = loggpsdata_send;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);    

  data_8 = fr_mode;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);   

  data_8 = se_mode;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char); 

  data_8 = Dwelltime;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);   

  data_8 = save_mode;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);  
  
  data_8 = save_sensor_mode;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);  
      
  data_8 = Intwk;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);  

  DATA.writeUInt(addr1, sys_time);               
  addr1 += sizeof(unsigned int);  
  
  for(uint8_t i=0;i<strlen(sys.blemask_data);i++)
  {
    DATA.writeUChar(addr1, blemask_data[i]);               
    addr1 += sizeof(unsigned char);
  }
      
  KEY.commit();
  DATA.commit();
  addr = 0;
  addr1 = 0;
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
//mtdc    
  mtdc = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);
  if(mtdc == 0)
    mtdc = 300000;
//atdc    
  atdc = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);
  if(atdc == 0)
    atdc = 60000;           
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

  fre = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);       

  frame_flag = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);    
  
  loggpsdata_send = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char); 

  fr_mode = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char); 

  se_mode = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);     

  Dwelltime = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);  

  save_mode = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);    

  save_sensor_mode = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);

  Intwk = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);

  sys_time = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);
  if(sys_time == 0)
    sys_time = 1200000;  

  for(uint8_t i=0;i<6;i++)
  {
    sys.blemask_data[i] = DATA.readUChar(addr1);
    addr1 += sizeof(unsigned char);
  }             
}

void SYS::gps_data_Weite(void)
{
//  if(addr_gps_write == 3765)
//  {
//   addr_gps_write = 0; 
//  }
   if(addr_gps_write <= 3765)
   {
      for(uint8_t i=0;i<15;i++)
      {
        GPSDATA.writeUChar(addr_gps_write, sys.gps_data_buff[i]);               
        addr_gps_write += sizeof(unsigned char);
      }         
      sys.config_Write();
      GPSDATA.commit();  
   } 
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

void customize_freq_set(void)
{
  #if defined( CFG_us915 ) || defined( CFG_au915 )
    // NA-US and AU channels 0-71 are configured automatically
    // but only one group of 8 should (a subband) should be active
    // TTN recommends the second sub band, 1 in a zero based count. 
    // We'll disable all 72 channels used by TTN
    for (int c = 0; c < 72; c++){
      LMIC_disableChannel(c);
    }
    // We'll only enable Channel 16 (905.5Mhz) since we're transmitting on a single-channel
    LMIC_enableChannel(sys.fre);   
  #elif defined( CFG_eu868 )
    // Set up the channels used by the Things Network, which corresponds
    // to the defaults of most gateways. Without this, only three base
    // channels from the LoRaWAN specification are used, which certainly
    // works, so it is good for debugging, but can overload those
    // frequencies, so be sure to configure the full frequency range of
    // your network here (unless your network autoconfigures them).
    // Setting up channels should happen after LMIC_setSession, as that
    // configures the minimal channel set. The LMIC doesn't let you change
    // the three basic settings, but we show them here.
    switch(sys.fre){
      case 0:
        LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      break;
      case 1:
        LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
      break;
      case 2:    
        LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      break;
      case 3:    
        LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      break;
      case 4:  
        LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      break;
      case 5:  
        LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      break;
      case 6:  
        LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      break;
      case 7:  
        LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      break;
      case 8:  
        LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band   
      break;
      default: 
      break; 
    }
    // TTN defines an additional channel at 869.525Mhz using SF9 for class B
    // devices' ping slots. LMIC does not have an easy way to define set this
    // frequency and support for class B is spotty and untested, so this
    // frequency is not configured here.
    #elif defined(CFG_as923)
    // Set up the channels used in your country. Only two are defined by default,
    // and they cannot be changed.  Use BAND_CENTI to indicate 1% duty cycle.
    switch(sys.fre){
      case 0:    
        LMIC_setupChannel(0, 923200000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
      break;
      case 1:         
        LMIC_setupChannel(1, 923400000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
      break;  
      default: 
      break; 
    }
    // ... extra definitions for channels 2..n here
    #elif defined(CFG_kr920)
    // Set up the channels used in your country. Three are defined by default,
    // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
    // BAND_MILLI.
    switch(sys.fre){
      case 0:     
        LMIC_setupChannel(0, 922100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break;
      case 1:         
        LMIC_setupChannel(1, 922300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break;
      case 2:         
        LMIC_setupChannel(2, 922500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break; 
      default: 
      break; 
    }
    // ... extra definitions for channels 3..n here.
    #elif defined(CFG_in866)
    // Set up the channels used in your country. Three are defined by default,
    // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
    // BAND_MILLI.
    switch(sys.fre){
      case 0:      
        LMIC_setupChannel(0, 865062500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break;
      case 1:       
        LMIC_setupChannel(1, 865402500, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break;
      case 2:       
        LMIC_setupChannel(2, 865985000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break;      
      default: 
      break; 
    }
    // ... extra definitions for channels 3..n here.
    #elif defined(CFG_kz865)
    // Set up the channels used in your country. Three are defined by default,
    // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
    // BAND_MILLI.
    switch(sys.fre){
      case 0:      
        LMIC_setupChannel(0, 865100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break;
      case 1:       
        LMIC_setupChannel(1, 865300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break;
      case 2:       
        LMIC_setupChannel(2, 865500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break; 
      default: 
      break; 
    }
    // ... extra definitions for channels 3..n here. 
    #elif defined(CFG_ma869)
    // Set up the channels used in your country. Three are defined by default,
    // and they cannot be changed. Duty cycle doesn't matter, but is conventionally
    // BAND_MILLI.
    switch(sys.fre){
      case 0:        
        LMIC_setupChannel(0, 869100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break;
      case 1:         
        LMIC_setupChannel(1, 869300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break;
      case 2:         
        LMIC_setupChannel(2, 869500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_MILLI);
      break; 
      default: 
      break; 
    }
    // ... extra definitions for channels 3..n here.
    #elif defined(CFG_ru864)
    // Set up the channels used in your country. Only two are defined by default,
    // and they cannot be changed.  Use BAND_CENTI to indicate 1% duty cycle.
    switch(sys.fre){
      case 0:      
        LMIC_setupChannel(0, 868900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
      break;
      case 1:       
        LMIC_setupChannel(1, 869100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
      break; 
      default: 
      break; 
    }    
    // ... extra definitions for channels 2..n here       
    #else
    # error Region not supported
    #endif
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
