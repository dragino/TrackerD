#include "common.h"

SYS sys;
Sensor sensor;
Devicet devicet;

//#define buzzer (4)
int freq = 2000;//设置频率
int channel = 0;//通道号，取值0 ~ 15
int resolution = 8;//计数位数，2的8次幂=256

uint32_t s_config[32] = {0};
uint8_t config_count =0;

uint32_t s_config1[32] = {0};
uint8_t config_count1 =0;

uint8_t addr_ble =0;

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
  float bat_f  =0.0;
  float batv_f = 0.0;
  if((sys.cdevaddr<=25692040)|| (sys.sensor_type == 13))
  {
    batv_f = 3.30/4095*analogRead(BAT_PIN_READ);
    if(batv_f == 3.3)
    {
      bat_f = 3.46/4095*analogRead(BAT_PIN_READ);
      bat_f = bat_f*570/470;
    }
    else
    {
      bat_f = batv_f*570/470;
    }
  }
  else
  {
      batv_f = 3.30/4095*analogRead(BAT_PIN_READ1);
      if(batv_f == 3.3)
      {
        bat_f = 3.46/4095*analogRead(BAT_PIN_READ1);
        bat_f = bat_f*570/470;
      }
      else
      {
        bat_f = batv_f*570/470;
      }
  }
//    Serial.printf("BAT:%.2f V\r\n",bat_f);  
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

void device_strcmp(void)
{
  Serial.printf("sys.cdevaddr=%d\r\n",sys.cdevaddr); 
  if((sys.cdevaddr<=25692040))
  { 
//    Serial.printf("button_event_init\r\n"); 
    button_event_init();
  }
  else if(sys.sensor_type == 22 || (sys.cdevaddr>25692040))   
  {   
//    Serial.printf("button_event_init\r\n"); 
    button_event_init1();
  }    
}

void button_loop(void)
{
  if((sys.cdevaddr<=25692040))
  {
//    Serial.printf("button_event_init\r\n"); 
      button_attach_loop();
  }
  else if(sys.sensor_type == 22 || (sys.cdevaddr>25692040))    
  {
//    Serial.printf("button_event_init1\r\n");   
      button_attach_loop1();
  }  

}

void gpio_reset(void)
{
//  gpio_reset_pin((gpio_num_t)0);
  gpio_reset_pin((gpio_num_t)2);
//  gpio_reset_pin((gpio_num_t)4);
  if(sys.GPS_flag ==1 )
   {
    gpio_reset_pin((gpio_num_t)12);    
   }
  gpio_reset_pin((gpio_num_t)13);
  gpio_reset_pin((gpio_num_t)14);
  gpio_reset_pin((gpio_num_t)15);
  gpio_reset_pin((gpio_num_t)25);
  gpio_reset_pin((gpio_num_t)26);
  gpio_reset_pin((gpio_num_t)27);
  gpio_reset_pin((gpio_num_t)32);
  gpio_reset_pin((gpio_num_t)33);
  gpio_reset_pin((gpio_num_t)5);
  gpio_reset_pin((gpio_num_t)18);
  gpio_reset_pin((gpio_num_t)19);
  
//  gpio_pullup_dis((gpio_num_t)0);
  gpio_pullup_dis((gpio_num_t)2);
//  gpio_pullup_dis((gpio_num_t)4);
  if(sys.GPS_flag ==1 )
   {
      gpio_pullup_dis((gpio_num_t)12);
   }
  gpio_pullup_dis((gpio_num_t)13);
  gpio_pullup_dis((gpio_num_t)14);
  gpio_pullup_dis((gpio_num_t)15);
  gpio_pullup_dis((gpio_num_t)25);
  gpio_pullup_dis((gpio_num_t)26);
  gpio_pullup_dis((gpio_num_t)27);
  gpio_pullup_dis((gpio_num_t)32);
  gpio_pullup_dis((gpio_num_t)33);
  gpio_pullup_dis((gpio_num_t)5);
  gpio_pullup_dis((gpio_num_t)18);
  gpio_pullup_dis((gpio_num_t)19);
}

void Device_status()
{
  uint8_t freq_band;
  uint16_t version;
  
  devicet.battrey = BatGet();
  if(sys.sensor_type == 13)
  {
    devicet.sensor_type = 0x13;
  }
  else if(sys.sensor_type == 22)  
  {
    devicet.sensor_type = 0x22;
  } 
  
  sys.fire_version = string_touint();
//  Serial.printf("sys.fire_version:%d\r\n",sys.fire_version);
  if(sys.fire_version>=100)
  {
    version=(sys.fire_version/100)<<8|(sys.fire_version/10%10)<<4|(sys.fire_version%10);
  }
  else
  {
    version=(sys.fire_version/10)<<8|(sys.fire_version%10)<<4;    
  }
  devicet.firm_ver = version;
//  Serial.printf("devicet.firm_ver:%d\r\n",devicet.firm_ver);
  #if defined(CFG_eu868)
    freq_band=0x01;
  #elif defined( CFG_us915)
    freq_band=0x02;
  #elif defined( CFG_in866 )
    freq_band=0x03;
  #elif defined( CFG_au915 )
    freq_band=0x04;
  #elif defined( CFG_kz865 )
    freq_band=0x05;
  #elif defined( CFG_ru864 )
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
  #elif defined( CFG_CN470 )
    freq_band=0x0B;
  #elif defined( CFG_EU433 )
    freq_band=0x0C;
  #elif defined( CFG_kr920 ) 
    freq_band=0x0D;
  #elif defined( CFG_ma869 ) 
    freq_band=0x0E;
  #else
    freq_band=0x00;
  #endif
  devicet.freq_band = freq_band;
  
  #if defined( CFG_us915 ) || defined( CFG_au915 )
  devicet.sub_band = sys.channel_single; 
  #else
  devicet.sub_band = 0xff;
  #endif

  devicet.SMODE = ((sys.sensor_mode <<6)|(sys.mod<<4 )|sys.ble_mod) & 0xFF;

  devicet.FLAG = ((sys.PNACKmd<<2)|(sys.lon<<1)|sys.Intwk)&0xFF;
  
}
void ABP_Band_information(void)
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
//        LMIC_disableChannel(0);
        LMIC_disableChannel(1);
        LMIC_disableChannel(2);
        LMIC_disableChannel(3);
        LMIC_disableChannel(4);
        LMIC_disableChannel(5);
        LMIC_disableChannel(6);
        LMIC_disableChannel(7);
        LMIC_disableChannel(8);         
      break;
      case 1:
        LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
         LMIC_disableChannel(0);
//        LMIC_disableChannel(1);
        LMIC_disableChannel(2);
        LMIC_disableChannel(3);
        LMIC_disableChannel(4);
        LMIC_disableChannel(5);
        LMIC_disableChannel(6);
        LMIC_disableChannel(7);
        LMIC_disableChannel(8);         
      break;
      case 2:    
        LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_disableChannel(0);
        LMIC_disableChannel(1);
//        LMIC_disableChannel(2);
        LMIC_disableChannel(3);
        LMIC_disableChannel(4);
        LMIC_disableChannel(5);
        LMIC_disableChannel(6);
        LMIC_disableChannel(7);
        LMIC_disableChannel(8);          
      break;
      case 3:    
        LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_disableChannel(0);
        LMIC_disableChannel(1);
        LMIC_disableChannel(2);
//        LMIC_disableChannel(3);
        LMIC_disableChannel(4);
        LMIC_disableChannel(5);
        LMIC_disableChannel(6);
        LMIC_disableChannel(7);
        LMIC_disableChannel(8);         
      break;
      case 4:  
        LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_disableChannel(0);
        LMIC_disableChannel(1);
        LMIC_disableChannel(2);
        LMIC_disableChannel(3);
//        LMIC_disableChannel(4);
        LMIC_disableChannel(5);
        LMIC_disableChannel(6);
        LMIC_disableChannel(7);
        LMIC_disableChannel(8);          
      break;
      case 5:  
        LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_disableChannel(0);
        LMIC_disableChannel(1);
        LMIC_disableChannel(2);
        LMIC_disableChannel(3);
        LMIC_disableChannel(4);
//        LMIC_disableChannel(5);
        LMIC_disableChannel(6);
        LMIC_disableChannel(7);
        LMIC_disableChannel(8);          
      break;
      case 6:  
        LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_disableChannel(0);
        LMIC_disableChannel(1);
        LMIC_disableChannel(2);
        LMIC_disableChannel(3);
        LMIC_disableChannel(4);
        LMIC_disableChannel(5);
//        LMIC_disableChannel(6);
        LMIC_disableChannel(7);
        LMIC_disableChannel(8);          
      break;
      case 7:  
        LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
        LMIC_disableChannel(0);
        LMIC_disableChannel(1);
        LMIC_disableChannel(2);
        LMIC_disableChannel(3);
        LMIC_disableChannel(4);
        LMIC_disableChannel(5);
        LMIC_disableChannel(6);
//        LMIC_disableChannel(7);
        LMIC_disableChannel(8);          
      break;
      case 8:  
        LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band   
        LMIC_disableChannel(0);
        LMIC_disableChannel(1);
        LMIC_disableChannel(2);
        LMIC_disableChannel(3);
        LMIC_disableChannel(4);
        LMIC_disableChannel(5);
        LMIC_disableChannel(6);
        LMIC_disableChannel(7);
//        LMIC_disableChannel(8);          
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
      
//    LMIC_setLinkCheckMode(0);
//    if(sys.LORA_GetADR() == ADR_ENABLE)
//    {
//      LMIC_setAdrMode(0);
//      LMIC_setDrTxpow(sys.LORA_GetDR(),sys.LORA_GetTXP());
//    }
//    else
//    {
//      sys.LORA_SetDR(LMIC.datarate);
//    }       
}

void ABP_InIt(void)
{
    uint8_t appskey_buff[16]={0};
    sys.LORA_GetAPPSKEY(appskey_buff); 
    uint8_t nwkskey_buff[16]={0};
    sys.LORA_GetNWKSKEY(nwkskey_buff);       
    uint8_t appskey[sizeof(appskey_buff)];
    uint8_t nwkskey[sizeof(nwkskey_buff)];
    memcpy_P(appskey, appskey_buff, sizeof(appskey_buff));
    memcpy_P(nwkskey, nwkskey_buff, sizeof(nwkskey_buff));
    LMIC_setSession (0x13, sys.LORA_GetDevaddr(), nwkskey_buff, appskey_buff);    

}

void SYS::Band_information(void)
{
  #if defined(CFG_eu868)
    Serial.println("EU868");
  #elif defined( CFG_us915)
    Serial.println("US915");
  #elif defined( CFG_in866 )
    Serial.println("IN865");
  #elif defined( CFG_au915 )
    Serial.println("AU915");
  #elif defined( CFG_kz865 )
    Serial.println("KZ865");
  #elif defined( CFG_ru864 )
    Serial.println("RU864");
  #elif defined( CFG_as923 )
    #if defined AS923_1
    Serial.println("AS923_1");
    #elif defined AS923_2
    Serial.println("AS923_2");
    #elif defined AS923_3
    Serial.println("AS923_3");
    #elif defined AS923_4
    Serial.println("AS923_4");    
    #else
    Serial.println("AS923");
    #endif
  #elif defined( REGION_CN470 )
    Serial.println("CN470");
  #elif defined( REGION_EU433 )
    Serial.println("EU433");
  #elif defined( CFG_kr920 ) 
    Serial.println("KR920");
  #elif defined( REGION_ma869 ) 
    Serial.println("MA869");
  #else
   Serial.println("********");
  #endif
}

uint16_t string_touint(void)
{
  char *p;  
  uint8_t chanum=0; 
  uint16_t versi;
  char version[8]="";
  if(sys.sensor_type == 13)
  { 
   p=Pro_version;
  }
  else if(sys.sensor_type == 22)
  {
    p=Pro_version1;
  }
  
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
  
  addr =68;
  
  KEY.writeUChar(addr,fire_version_write);               
  addr += sizeof(unsigned char); 
     
  data_8 = sensor_type;
  KEY.writeUChar(addr, data_8);               
  addr += sizeof(unsigned char);  
  
  data_32 =cdevaddr;
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

  DATA.writeUInt(addr1, gps_write);               
  addr1 += sizeof(unsigned int);    

  data_8 = exit_off;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char); 

//ble_mod
  data_8 = ble_mod;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);  
//mod
  data_8 = save_ble_mode;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);  

  data_8 =  PNACKmd;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  data_8 =  showid;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);  

  data_8 = pdop_value;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);   

  DATA.writeUChar(addr1, sys.TF[0]);               
  addr1 += sizeof(unsigned char);  

  DATA.writeUChar(addr1,FDR_flag);               
  addr1 += sizeof(unsigned char); 
  
  data_8 = atst;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char); 

  data_8 = pedometer;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  data_8 = fall_detection;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);  
  
  data_8 = ble_gps;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char); 

  DATA.writeUInt(addr1, exit_alarm_time);               
  addr1 += sizeof(unsigned int); 

  data_8 = beep_flag;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char); 

  data_8 = njm;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  data_8 = GPS_flag;
  DATA.writeUChar(addr1, data_8);               
  addr1 += sizeof(unsigned char);

  if(sys.blemask_flag == 1)
  {
    addr1 =75;
    for(int i=0;i<3;i++)
    {
      DATA.writeUInt(addr1, 0);          
      addr1 += sizeof(unsigned int);
    }     
    addr1 =75;
    for(int i=0,j = 0;i<strlen(sys.blemask_data);i=i+4,j++)
    {
      DATA.writeUInt(addr1, s_config[config_count++]=(sys.blemask_data[i+0]<<24)|(sys.blemask_data[i+1]<<16)|(sys.blemask_data[i+2]<<8)|(sys.blemask_data[i+3]));          
      addr1 += sizeof(unsigned int);
    }  
    addr_ble =  addr1; 
    sys.blemask_flag = 0;  
  }  
  if(sys.blemask_flag == 2)
  {
    addr1 = 89;
    for(int i=0;i<3;i++)
    {
      DATA.writeUInt(addr1, s_config1[0]=0);          
      addr1 += sizeof(unsigned int);
    }  
    addr1 = 89;    
    for(int i=0,j = 0;i<strlen(sys.wifimask_data);i=i+4,j++)
    {   
      DATA.writeUInt(addr1, s_config1[config_count1++]=(sys.wifimask_data[i+0]<<24)|(sys.wifimask_data[i+1]<<16)|(sys.wifimask_data[i+2]<<8)|(sys.wifimask_data[i+3]));               
      addr1 += sizeof(unsigned int); 
    }      
    sys.blemask_flag = 0;
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
  
  addr =68;
  fire_version_write = KEY.readUChar(addr);
  addr += sizeof(unsigned char);

  data_8 = KEY.readByte(addr);      
  sensor_type = data_8;         
  addr += sizeof(byte);

  cdevaddr = KEY.readUInt(addr);
  addr += sizeof(unsigned int);  
//tdc
  tdc = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);
  if(tdc == 0 || tdc == 4294487296)
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
//  if(Positioning_time == 0)
//    Positioning_time = 180000;  

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

  gps_write = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);

  exit_off = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);  

  ble_mod = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);    

  save_ble_mode = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);

  PNACKmd = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);   

  showid = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char); 

  pdop_value = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char);     

  sys.TF[0] = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char); 

  FDR_flag = DATA.readUChar(addr1);
  addr1 += sizeof(unsigned char); 

  data_8 = DATA.readByte(addr1);      
  atst = data_8;         
  addr1 += sizeof(byte); 

  data_8 = DATA.readByte(addr1);      
  pedometer = data_8;         
  addr1 += sizeof(byte); 

  data_8 = DATA.readByte(addr1);      
  fall_detection = data_8;         
  addr1 += sizeof(byte); 
  
  data_8 = DATA.readByte(addr1);      
  ble_gps = data_8;         
  addr1 += sizeof(byte);
  
  exit_alarm_time = DATA.readUInt(addr1);
  addr1 += sizeof(unsigned int);
    
  data_8 = DATA.readByte(addr1);      
  beep_flag = data_8;         
  addr1 += sizeof(byte);

  data_8 = DATA.readByte(addr1);      
  njm = data_8;         
  addr1 += sizeof(byte);

  data_8 = DATA.readByte(addr1);      
  GPS_flag = data_8;         
  addr1 += sizeof(byte);

  for(uint8_t i=0,j = 0;i<3;i++,j=j+4)
  {
    s_config[i] = DATA.readUInt(addr1);
    addr1 += sizeof(unsigned int); 
    blemask_data[j]= s_config[0+i]>>24;
    blemask_data[j+1]= s_config[0+i]>>16;
    blemask_data[j+2]= s_config[0+i]>>8;
    blemask_data[j+3]= s_config[0+i];
  } 
  addr1 = 89;
  for(uint8_t i=0,j = 0;i<3;i++,j=j+4)
  {
    s_config1[i] = DATA.readUInt(addr1);
    addr1 += sizeof(unsigned int); 
    wifimask_data[j]= s_config1[0+i]>>24;
    wifimask_data[j+1]= s_config1[0+i]>>16;
    wifimask_data[j+2]= s_config1[0+i]>>8;
    wifimask_data[j+3]= s_config1[0+i];
  }           
}

void SYS::gps_data_Weite(void)
{
  if(sys.sensor_type == 13)
  {
    if(addr_gps_write >= 4095)
    {
     gps_write = 4095;
     addr_gps_write = 0; 
    }
   if(addr_gps_write <= 4095)
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
  else if(sys.sensor_type == 22)  
  {
    if(addr_gps_write >= 4080)
    {
     gps_write = 4080;
     addr_gps_write = 0; 
    }
   if(addr_gps_write <= 4080)
    {
        for(uint8_t i=0;i<17;i++)
        {
          GPSDATA.writeUChar(addr_gps_write, sys.gps_data_buff[i]);               
          addr_gps_write += sizeof(unsigned char);
        }         
        sys.config_Write();
        GPSDATA.commit();  
    } 
  } 
      

}

void SYS::gps_data_Read(void)
{
  if(sys.sensor_type == 13)
  {
    for(uint8_t i=0;i<15;i++)
    {
      sys.gps_data_buff[i] = GPSDATA.readUChar(addr_gps_read);
      addr_gps_read += sizeof(unsigned char);
    }
  }
  else  if(sys.sensor_type == 22)
  {
    for(uint8_t i=0;i<17;i++)
    {
      sys.gps_data_buff[i] = GPSDATA.readUChar(addr_gps_read);
      addr_gps_read += sizeof(unsigned char);
    }    
  }
}


void SYS::gps_pdta_Read(uint32_t pdta_addr)
{
  for(uint32_t i=0;i<pdta_addr;i++)
  {
    if(sys.sensor_type == 13)
    {
      for(uint8_t i=0;i<15;i++)
      {
        sys.gps_data_buff[i] = GPSDATA.readUChar(addr_pdta_read);
        addr_pdta_read += sizeof(unsigned char);
      }
       for(int i=0;i<15;i++)
          Serial.printf("%.2x ",sys.gps_data_buff[i]);
      Serial.println();     
    }
    else  if(sys.sensor_type == 22)
    {
      for(uint8_t i=0;i<17;i++)
      {
        sys.gps_data_buff[i] = GPSDATA.readUChar(addr_pdta_read);
        addr_pdta_read += sizeof(unsigned char);
        addr_gps_read =0;
      }
       for(int i=0;i<17;i++)
          Serial.printf("%.2x ",sys.gps_data_buff[i]);
      Serial.println(); 
      Serial.printf("addr_pdta_read:%d\r\n",sys.addr_pdta_read);          
    }
  }
  addr_pdta_read =0; 
}

void SYS::read_gps_data_on_flash()
{
  gps_data_Read();
  Serial.print("GPS DATA:");
 if(sys.sensor_type == 13)
 {  
    for(int i=0;i<15;i++)
        Serial.printf("%.2x ",sys.gps_data_buff[i]);
    Serial.println();
 }
 else if(sys.sensor_type == 22)
 {
    for(int i=0;i<17;i++)
        Serial.printf("%.2x ",sys.gps_data_buff[i]);
    Serial.println();  
 }
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
