#include "at.h"

static uint8_t  keep = 0;
/**************       AT_ERROR       **************/
ATEerror_t at_return_error(const char *param)
{
  return AT_ERROR;
}

/**************       AT?       **************/
ATEerror_t at_que(const char *param)
{
  Serial.println("\r\nAT+<CMD>?        : Help on <CMD>\r\n"
              "AT+<CMD>         : Run <CMD>\r\n"
              "AT+<CMD>=<value> : Set the value\r\n"
              "AT+<CMD>=?       : Get the value\r\n");
  for (int i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
  {
    Serial.println(ATCommand[i].help_string);
  } 
  
  return AT_OK;
}

/**************       AT_MODEL       **************/
ATEerror_t at_model_get(const char *param)
{
  if(keep)
    Serial.print(AT MODEL"=");
  Serial.println(Pro_model "," Pro_version);
  return AT_OK;
}

/**************       ATZ       **************/
ATEerror_t at_reset_run(const char *param)
{
  ESP.restart();
  return AT_OK;
}

/**************       AT+FDR       **************/
ATEerror_t at_fdr_run(const char *param)
{
  int addr = EEPROM_KEY_SIZE;
  for(int i=EEPROM_KEY_SIZE;i<EEPROM_SIZE;i++)
  {
    EEPROM.writeByte(addr, 0);               
    addr += sizeof(byte);
  }
  
  EEPROM.commit(); 
  addr = 0;
  ESP.restart();
  return AT_OK;
}

/**************       AT_DEUI       **************/
ATEerror_t at_deui_get(const char *param)
{
  if(keep)
    Serial.print(AT DEUI"=");
  for(uint8_t i=0;i<8;i++) 
    Serial.printf("%.2x ",sys.deveui[i]);
  Serial.println();
  return AT_OK;
}

ATEerror_t at_deui_set(const char *param)
{
  char* pos = strchr(param,'=');
  if(strlen(&pos[1])!=16)
    return AT_PARAM_ERROR;
  char d_buff[16]={0};
  StrToHex(d_buff,&pos[1],16);
  memcpy(sys.deveui,d_buff,8);
  return AT_OK;
}
/**************       AT_APPEUI       **************/
ATEerror_t at_appeui_get(const char *param)
{
  if(keep)
    Serial.print(AT APPEUI"=");
  for(uint8_t i=0;i<8;i++) 
    Serial.printf("%.2x ",sys.appeui[i]);
  Serial.println();
  return AT_OK;
}

ATEerror_t at_appeui_set(const char *param)
{
  char* pos = strchr(param,'=');
  if(strlen(&pos[1])!=16)
    return AT_PARAM_ERROR;
  char d_buff[16]={0};
  StrToHex(d_buff,&pos[1],16);
  memcpy(sys.appeui,d_buff,8);
  return AT_OK;
}
/**************       AT_APPKEY       **************/
ATEerror_t at_appkey_get(const char *param)
{
  if(keep)
    Serial.print(AT APPKEY"=");
  for(uint8_t i=0;i<16;i++) 
    Serial.printf("%.2x ",sys.appkey[i]);
  Serial.println();
  return AT_OK;
}

ATEerror_t at_appkey_set(const char *param)
{
  char* pos = strchr(param,'=');
  if(strlen(&pos[1])!=32)
    return AT_PARAM_ERROR;
  char d_buff[32]={0};
  StrToHex(d_buff,&pos[1],32);
  memcpy(sys.appkey,d_buff,16);
  return AT_OK;
}
/**************       AT_DADDR       **************/
ATEerror_t at_daddr_get(const char *param)
{
  if(keep)
    Serial.print(AT DADDR"=");
  Serial.println(sys.devaddr, HEX);
  return AT_OK;
}

ATEerror_t at_daddr_set(const char *param)
{
  char* pos = strchr(param,'=');
  sys.devaddr = atoi((param+(pos-param)+1));  
  return AT_OK;
}
/**************       AT_APPSKEY       **************/
ATEerror_t at_appskey_get(const char *param)
{
  if(keep)
    Serial.print(AT APPSKEY"=");
  for(uint8_t i=0;i<16;i++) 
    Serial.printf("%.2x ",sys.appSKey[i]);
  Serial.println();
  return AT_OK;
}

ATEerror_t at_appskey_set(const char *param)
{
  char* pos = strchr(param,'=');
  if(strlen(&pos[1])!=32)
    return AT_PARAM_ERROR;
  char d_buff[32]={0};
  StrToHex(d_buff,&pos[1],32);
  memcpy(sys.appSKey,d_buff,16);
  return AT_OK;
}
/**************       AT_NWKSKEY       **************/
ATEerror_t at_nwkskey_get(const char *param)
{
  if(keep)
    Serial.print(AT NWKSKEY"=");
  for(uint8_t i=0;i<16;i++) 
    Serial.printf("%.2x ",sys.nwkSKey[i]);
  Serial.println();
  return AT_OK;
}

ATEerror_t at_nwkskey_set(const char *param)
{
  char* pos = strchr(param,'=');
  if(strlen(&pos[1])!=32)
    return AT_PARAM_ERROR;
  char d_buff[32]={0};
  StrToHex(d_buff,&pos[1],32);
  memcpy(sys.nwkSKey,d_buff,16);
  return AT_OK;
}
/**************       AT_ADR       **************/
ATEerror_t at_adr_get(const char *param)
{
  return AT_OK;
}
ATEerror_t at_adr_set(const char *param)
{
  return AT_OK;
}
/**************       AT_CFG       **************/
ATEerror_t at_cfg_run(const char *param)
{
  keep = 1;
  for (int i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
  {
    ATCommand[i].get(param);
  }
  keep = 0;
  return AT_OK;
}

/**************       AT_SCWIFI       **************/
ATEerror_t at_scwifi_run(const char *param)
{
  wifi_scan();
  return AT_OK;
}

/**************       AT_TDC       **************/
ATEerror_t at_tdc_get(const char *param)
{
  if(keep)
    Serial.print(AT TDC "=");
  Serial.println(sys.tdc);
  return AT_OK;
}
ATEerror_t at_tdc_set(const char *param)
{
  char* pos = strchr(param,'=');
  uint32_t tdc = atoi((param+(pos-param)+1));
  if(tdc > 0xFFFFFFFF || tdc<60)
  {
    return AT_PARAM_ERROR;
  }
  sys.tdc = tdc;
  return AT_OK;
}

/**************       AT_SYSMES       **************/
ATEerror_t at_sysmes_run(const char *param)
{
  Serial.print("flash size:");Serial.println(ESP.getFlashChipSize()); 
  Serial.print("flash working mode:");Serial.println(ESP.getFlashChipMode()); 
  Serial.print("Current firmware size:");Serial.println(ESP.getSketchSize());  
  Serial.print("Clock frequency:"); Serial.println(ESP.getCpuFreqMHz());
 
  return AT_OK;
}

/**************       ATInsPro       **************/
ATEerror_t ATInsPro(char* atdata)
{
  uint8_t i = 0;
  rtrim(atdata);
  if(strcmp(atdata,AT) == 0)
    return AT_OK;
  else if(strcmp(atdata,AT"?") == 0)
  {
    return at_que(atdata);
  }
  
  while(strstr(atdata,ATCommand[i].string) == NULL)
  {
    i++;
    if(i>(sizeof(ATCommand) / sizeof(struct ATCommand_s))-1)
    {
      return AT_ERROR;
    }
  }

  if(strstr(atdata,"=?"))
  { 
     return ATCommand[i].get(atdata);
  }
  else if(strstr(atdata,"="))
  {
    ATEerror_t AT_State = ATCommand[i].set(atdata);
    if(AT_State == AT_OK)
      sys.config_Set();
    else
      return AT_State;
  }
  else if(strstr(atdata,"?"))
  {
    Serial.println(ATCommand[i].help_string);
  }
  else 
  {
    return ATCommand[i].run(atdata);
  }
  return AT_OK;
}
