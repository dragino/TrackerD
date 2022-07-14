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

/**************       AT_MOD       **************/
ATEerror_t at_mod_get(const char *param)
{
  if(keep)
    Serial.print(AT MOD"=");
  Serial.printf("%d\r\n",sys.mod);
  return AT_OK;
}

ATEerror_t at_mod_set(const char *param)
{
  sys.mod = atoi(param);
//  switch (sys.mod)
//  {
//    case DEFAULT_MODE:
//      Serial.printf("default mode");
//    break;
//    case MOVE_DETECT_MODE:
//      Serial.printf("movement detect mode");
//    break;
//    case COLLISION_DETECT_MODE:
//      Serial.printf("Collision detect mode");
//    break;
//    case CUSTOM_MOVE_DETECT_MODE:
//      Serial.printf("custom movement detect mode");
//    break;
//    case GXHT3x_MODE:
//      Serial.printf("Tem and Hum mode");
//    break;
//    default:
//      Serial.printf("unknown mode");
//    break;
//  }
  return AT_OK;
}

/**************       AT_SMOD       **************/
ATEerror_t at_smod_get(const char *param)
{
  if(keep)
    Serial.print(AT SMOD"=");
  Serial.printf("%d\r\n",sys.sensor_mode);
  return AT_OK;
}

ATEerror_t at_smod_set(const char *param)
{
  sys.sensor_mode = atoi(param);
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
  
  sys.DATA_CLEAR(); 
//  EEPROM.commit(); 
  ESP.restart();
  return AT_OK;
}

/**************       AT_DEUI       **************/
ATEerror_t at_deui_get(const char *param)
{
  if(keep)
    Serial.print(AT DEUI"=");
  uint8_t d_buff[8]={0};
  uint8_t d_buff1[8]={0};
  sys.LORA_GetDEUI(d_buff);
  for(int i=7,j=0; i>=0,j<8; i--,j++)
  {
    d_buff1[j]=d_buff[i];
  }
  for(uint8_t i=0;i<8;i++) 
    Serial.printf("%.2x ",d_buff1[i]);
  Serial.println();      
  return AT_OK;
}

ATEerror_t at_deui_set(const char *param)
{
  if(strlen(param)!=16)
    return AT_PARAM_ERROR;
  uint8_t d_buff[16]={0};
  uint8_t d_buff1[16]={0};
  StrToHex(d_buff,(char*)param,16);
  for(int i=7,j=0; i>=0,j<8; i--,j++)
  {
    d_buff1[j]=d_buff[i];
  } 
  for(uint8_t i=0;i<8;i++) 
    Serial.printf("%.2x ",d_buff1[i]);
  Serial.println();     
  sys.LORA_SetDEUI(d_buff1);
  return AT_OK;
}
/**************       AT_APPEUI       **************/
ATEerror_t at_appeui_get(const char *param)
{
  if(keep)
    Serial.print(AT APPEUI"=");
  uint8_t app_buff[8]={0};
  uint8_t app_buff1[8]={0};
  sys.LORA_GetAEUI(app_buff);
   for(int i=7,j=0; i>=0,j<8; i--,j++)
  {
    app_buff1[j]=app_buff[i];
  }
  for(uint8_t i=0;i<8;i++) 
    Serial.printf("%.2x ",app_buff1[i]);
  Serial.println();  
  return AT_OK;
}

ATEerror_t at_appeui_set(const char *param)
{
  if(strlen(param)!=16)
    return AT_PARAM_ERROR;
  uint8_t app_buff[16]={0};
  uint8_t app_buff1[16]={0};
  StrToHex(app_buff,(char*)param,16);
   for(int i=7,j=0; i>=0,j<8; i--,j++)
  {
    app_buff1[j]=app_buff[i];
  }
  for(uint8_t i=0;i<8;i++) 
    Serial.printf("%.2x ",app_buff1[i]);
  Serial.println();    
  sys.LORA_SetAEUI(app_buff1);
  return AT_OK;
}
/**************       AT_APPKEY       **************/
ATEerror_t at_appkey_get(const char *param)
{
  if(keep)
    Serial.print(AT APPKEY"=");
  uint8_t akey_buff[16]={0};
  sys.LORA_GetAKEY(akey_buff);
  for(uint8_t i=0;i<16;i++) 
    Serial.printf("%.2x ",akey_buff[i]);
  Serial.println();
  return AT_OK;
}

ATEerror_t at_appkey_set(const char *param)
{
  if(strlen(param)!=32)
    return AT_PARAM_ERROR;
  uint8_t akey_buff[32]={0};
  StrToHex(akey_buff,(char*)param,32);
  sys.LORA_SetAKEY(akey_buff);
  return AT_OK;
}
/**************       AT_DADDR       **************/
ATEerror_t at_daddr_get(const char *param)
{
  if(keep)
    Serial.print(AT DADDR"=");
  Serial.printf("%x\r\n",sys.LORA_GetDevaddr());
  return AT_OK;
}

ATEerror_t at_daddr_set(const char *param)
{
  uint32_t daddr = atoi(param);  
  sys.LORA_SetDevaddr(daddr);
  return AT_OK;
}
/**************       AT_APPSKEY       **************/
ATEerror_t at_appskey_get(const char *param)
{
  if(keep)
    Serial.print(AT APPSKEY"=");
  uint8_t appskey_buff[16]={0};
  sys.LORA_GetAPPSKEY(appskey_buff);
  for(uint8_t i=0;i<16;i++)
    Serial.printf("%.2x ",appskey_buff[i]);
  Serial.println();
  return AT_OK;
}

ATEerror_t at_appskey_set(const char *param)
{
  if(strlen(param)!=32)
    return AT_PARAM_ERROR;
  uint8_t appskey_buff[32]={0};
  StrToHex(appskey_buff,(char*)param,32);
  sys.LORA_SetAPPSKEY(appskey_buff);
  return AT_OK;
}
/**************       AT_NWKSKEY       **************/
ATEerror_t at_nwkskey_get(const char *param)
{
  if(keep)
    Serial.print(AT NWKSKEY"=");
  uint8_t nwkskey_buff[16]={0};
  sys.LORA_GetNWKSKEY(nwkskey_buff);
  for(uint8_t i=0;i<16;i++) 
    Serial.printf("%.2x ",nwkskey_buff[i]);
  Serial.println();
  return AT_OK;
}

ATEerror_t at_nwkskey_set(const char *param)
{
  if(strlen(param)!=32)
    return AT_PARAM_ERROR;
  uint8_t nwkskey_buff[32]={0};
  StrToHex(nwkskey_buff,(char*)param,32);
  sys.LORA_SetNWKSKEY(nwkskey_buff);
  return AT_OK;
}
/**************       AT_ADR       **************/
ATEerror_t at_adr_get(const char *param)
{
  if(keep)
    Serial.print(AT ADR"=");
  bool _adr_flag = sys.LORA_GetADR();
  Serial.println(_adr_flag);
  return AT_OK;
}

ATEerror_t at_adr_set(const char *param)
{
  bool _adr = atoi(param);
  if(_adr!=0 && _adr!=1)
  {
    return AT_PARAM_ERROR;
  }
  sys.LORA_SetADR(_adr);
  return AT_OK;
}
/**************       AT_DR       **************/
ATEerror_t at_dr_get(const char *param)
{
  if(keep)
    Serial.print(AT DR"=");
  uint8_t _dr = sys.LORA_GetDR();
  Serial.println(_dr);
  return AT_OK;
}

ATEerror_t at_dr_set(const char *param)
{
  uint8_t _dr= atoi(param);
  if(_dr>=8)
  {
    return AT_PARAM_ERROR;
  }
  sys.LORA_SetDR(_dr);
  return AT_OK;
}
/**************       AT_TXP       **************/
ATEerror_t at_txp_get(const char *param)
{
  if(keep)
    Serial.print(AT TXP"=");
  uint8_t _txp = sys.LORA_GetTXP();
  Serial.println(_txp, DEC);
  return AT_OK;
}

ATEerror_t at_txp_set(const char *param)
{
  uint8_t _txp= atoi(param);
  if(_txp>5)
  {
    return AT_PARAM_ERROR;
  }
  sys.LORA_SetTXP(_txp);
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

/**************       AT_PDTA      **************/

ATEerror_t at_PDTA_set(const char *param)
{
  ;
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
  uint32_t tdc = atoi(param);
  if(tdc > 0xFFFFFFFF || tdc<60)
  {
    return AT_PARAM_ERROR;
  }
  sys.tdc = tdc;
  return AT_OK;
}

/**************       AT_FTIME      **************/
ATEerror_t at_ftime_get(const char *param)
{
  if(keep)
    Serial.print(AT FTIME "=");
  Serial.println(sys.Positioning_time);
  return AT_OK;
}
ATEerror_t at_ftime_set(const char *param)
{
  uint32_t Positioning_time = atoi(param);
  if(Positioning_time > 0xFFFFFFFF || Positioning_time<60)
  {
    return AT_PARAM_ERROR;
  }
  sys.Positioning_time = Positioning_time;
  return AT_OK;
}

/**************       AT_LON       **************/
ATEerror_t at_lon_get(const char *param)
{
  if(keep)
    Serial.print(AT LON "=");
  Serial.println((sys.lon==2?0:1));
  return AT_OK;
}
ATEerror_t at_lon_set(const char *param)
{
  uint32_t is_on = atoi(param);
  if(is_on == 0)
    is_on = 2;
  
  if(is_on != 2 || is_on!=1)
  {
    return AT_PARAM_ERROR;
  }
  sys.lon = is_on;
  return AT_OK;
}

/**************       AT_FRAME       **************/
ATEerror_t at_frame_get(const char *param)
{
  if(keep)
    Serial.print(AT FRAME  "=");
  Serial.println(sys.frame_flag);
  return AT_OK;
}
ATEerror_t at_frame_set(const char *param)
{
 uint8_t frame_flag = atoi(param);
  if(frame_flag > 2)
  {
    return AT_PARAM_ERROR;
  }
  sys.frame_flag = frame_flag;
  return AT_OK;
}

/**************       AT_CHE       **************/
ATEerror_t at_CHE_get(const char *param)
{
  if(keep)
    Serial.print(AT CHE "=");
  Serial.println(sys.channel_single);
  return AT_OK;
}
ATEerror_t at_CHE_set(const char *param)
{
  uint32_t is_on = atoi(param);
  
  if(is_on > 7)
  {
    return AT_PARAM_ERROR;
  }
  sys.channel_single = is_on;
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

  atdata+=2;
  if(strstr(atdata,"=?"))
  { 
     return ATCommand[i].get(atdata);
  }
  else if(strstr(atdata,"="))
  {
    atdata += ATCommand[i].size_string+1;
    ATEerror_t AT_State = ATCommand[i].set(atdata);
    if(AT_State == AT_OK)
      sys.config_Write();
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
