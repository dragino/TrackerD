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
//  sys.GPSDATA_CLEAR();
//  EEPROM.commit(); 
  ESP.restart();
  return AT_OK;
}

/**************       AT+SLEEP       **************/
ATEerror_t at_sleep_run(const char *param)
{
//  if(keep)
  Serial.println("sleep");  
  hal_sleep();    
  sys.LORA_EnterSleepMode();
  esp_deep_sleep_start(); //DeepSleep  
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
  Serial.printf("%08x\r\n",sys.LORA_GetDevaddr());
  return AT_OK;
}

ATEerror_t at_daddr_set(const char *param)
{
  char daddr[8] ={0};
  memcpy(daddr,param,8);
  uint8_t aa[4];
  StrToHex(aa,daddr,4);
  uint32_t devaddr = aa[0]<<24|aa[1]<<16|aa[2]<<8|aa[3];
  sys.LORA_SetDevaddr(devaddr);
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
  if(_adr_flag == 1)
  {
    Serial.println(0);
  }
  else if(_adr_flag == 0)
  {
    Serial.println(1); 
  }
  return AT_OK;
}

ATEerror_t at_adr_set(const char *param)
{
  bool _adr = atoi(param);
  if(_adr!=0 && _adr!=1)
  {
    return AT_PARAM_ERROR;
  }
  bool adr;
  if(_adr == 0)
  {
    adr = 1;
  }
  else if(_adr == 1)
  {
    adr = 0;
  }
  sys.LORA_SetADR(adr);
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
  if(strlen(param)!=1)
    return AT_PARAM_ERROR;
  if(_dr>=8)
  {
    return AT_PARAM_ERROR;
  }
  Serial.println("Attention:Take effect after ATZ");
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
  if(strlen(param)!=1)
    return AT_PARAM_ERROR;
  if(_txp>5)
  {
    return AT_PARAM_ERROR;
  }
  sys.LORA_SetTXP(_txp);
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

/**************       AT_SMOD       **************/
ATEerror_t at_smod_get(const char *param)
{
  if(keep)
    Serial.print(AT SMOD"=");
  Serial.printf("%d",sys.sensor_mode);
  Serial.printf(",");
  Serial.printf("%d",sys.mod);
  Serial.printf(",");
  Serial.printf("%d\r\n",sys.ble_mod);  
  return AT_OK;
}

ATEerror_t at_smod_set(const char *param)
{
  char mod[5]={0};
  memcpy(mod,param,5);
  sys.sensor_mode = (int)mod[0]-48;
  sys.save_sensor_mode = (int)mod[0]-48;
  sys.mod = (int)mod[2]-48;
  sys.save_mode = (int)mod[2]-48;
  sys.ble_mod = (int)mod[4]-48;
  sys.save_ble_mode = (int)mod[4]-48;    
  if(strlen(param)!=5)
    return AT_PARAM_ERROR;  
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
  sys.sys_time = tdc;
  return AT_OK;
}

/**************       AT_MTDC       **************/
ATEerror_t at_mtdc_get(const char *param)
{
  if(keep)
    Serial.print(AT MTDC "=");
  Serial.println(sys.mtdc);
  return AT_OK;
}
ATEerror_t at_mtdc_set(const char *param)
{
  uint32_t mtdc = atoi(param);

  if(mtdc > 0xFFFFFFFF || mtdc<60)
  {
    return AT_PARAM_ERROR;
  }
  sys.mtdc = mtdc;
  return AT_OK;
}


/**************       AT_ATDC       **************/
ATEerror_t at_atdc_get(const char *param)
{
  if(keep)
    Serial.print(AT ATDC "=");
  Serial.println(sys.atdc);
  return AT_OK;
}
ATEerror_t at_atdc_set(const char *param)
{
  uint32_t atdc = atoi(param);

  if(atdc > 0xFFFFFFFF)
  {
    return AT_PARAM_ERROR;
  }
  sys.atdc = atdc;
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
//  if(Positioning_time > 0xFFFFFFFF || Positioning_time<60)
//  {
//    return AT_PARAM_ERROR;
//  }
  sys.Positioning_time = Positioning_time*1000;
  return AT_OK;
}

/**************       AT_LON       **************/
ATEerror_t at_lon_get(const char *param)
{
  if(keep)
    Serial.print(AT LON "=");
  Serial.println(sys.lon);
  return AT_OK;
}
ATEerror_t at_lon_set(const char *param)
{
  uint32_t is_on = atoi(param); 
  if(is_on >=2)
  {
    return AT_PARAM_ERROR;
  }
  sys.lon = is_on;
  return AT_OK;
}

/**************       AT_INTWK       **************/
ATEerror_t at_intwk_get(const char *param)
{
  if(keep)
    Serial.print(AT INTWK "=");
  Serial.println(sys.Intwk);
  return AT_OK;
}
ATEerror_t at_intwk_set(const char *param)
{
  uint32_t intwk = atoi(param);
  
  if(intwk >=2)
  {
    return AT_PARAM_ERROR;
  }
  sys.Intwk = intwk;
  return AT_OK;
}

/**************       AT_cfm       **************/
ATEerror_t at_cfm_get(const char *param)
{
  if(keep)
    Serial.print(AT CFM  "=");
  Serial.println(sys.frame_flag);
  return AT_OK;
}
ATEerror_t at_cfm_set(const char *param)
{
 uint8_t frame_flag = atoi(param);
  if(frame_flag > 2)
  {
    return AT_PARAM_ERROR;
  }
  sys.frame_flag = frame_flag;
  return AT_OK;
}

/**************       AT_PNACKMD       **************/
ATEerror_t at_pnackmd_get(const char *param)
{
  if(keep)
    Serial.print(AT PNACKMD  "=");
  Serial.println(sys.PNACKmd);
  return AT_OK;
}
ATEerror_t at_pnackmd_set(const char *param)
{
 uint8_t pnackmd = atoi(param);
  if(pnackmd > 2)
  {
    return AT_PARAM_ERROR;
  }
  sys.PNACKmd = pnackmd;
  return AT_OK;
}

/**************       AT_NMEA353       **************/
ATEerror_t at_NMEA353_get(const char *param)
{
  if(keep)
   Serial.print(AT NMEA353  "=");
  Serial.println(sys.se_mode);
  return AT_OK;
}
ATEerror_t at_NMEA353_set(const char *param)
{
 uint8_t se_mode = atoi(param);
  if(se_mode > 4 || sys.ic_versino !=1)
  {
    return AT_PARAM_ERROR;
  }
  sys.se_mode = se_mode;
  return AT_OK;
}

/**************       AT_NMEA886       **************/
ATEerror_t at_NMEA886_get(const char *param)
{
  if(keep)
    Serial.print(AT NMEA886  "=");
  Serial.println(sys.fr_mode);
  return AT_OK;
}
ATEerror_t at_NMEA886_set(const char *param)
{
 uint8_t fr_mode = atoi(param);
  sys.fr_mode = fr_mode;
  return AT_OK;
}

/**************       AT_CHE       **************/
ATEerror_t at_CHE_get(const char *param)
{
  if(keep)
    Serial.print(AT CHE "=");
  Serial.println(sys.channel_single);
//  double frel;
//  double j,k,l;
//  #if defined( CFG_us915 )
//  if(sys.channel_single == 8)
//  {
//    j=902.2; k=0.1;l=1.6;
//  }
//  else
//  {
//    j=902.3;k=1.6;l=0.2;
//  }
//  #else
//  if(sys.channel_single == 9)
//  {
//    j=915.0;k=0.1;l=1.6;
//  }
//  else
//  {
//    j=915.2;k=1.6;l=0.2;
//  }
//  #endif
//  if(sys.channel_single)
//  {
//    frel=j+(sys.channel_single-1)*k;
//    for(int i=0;i<8;i++)
//    {
//      Serial.printf("%0.1f",frel);
//      frel=frel+1;
//    }
//    Serial.printf("\n\r");
//  }
//  else
//  {
//     Serial.println("Use default channel");
//  }
  return AT_OK;
}
ATEerror_t at_CHE_set(const char *param)
{
  uint32_t is_on = atoi(param);
  if(strlen(param)!=1)
    return AT_PARAM_ERROR; 
//  #if defined( CFG_us915 )
  if(is_on > 9)
  {
//    is_on = 1;
    AT_PARAM_ERROR;
  }    
//  else
//  {
//    Serial.println("Attention:Take effect after ATZ");
//  }
//  #elif defined( CFG_au915 )
//  if(is_on > 9)
//  {
//    is_on = 1;
//    Serial.println("Error Subband, must be 0 ~ 8");
//  }    
//  else
//  {
    Serial.println("Attention:Take effect after ATZ");
//  }
//  #else
//  is_on = 0;
//  #endif  
  sys.channel_single = is_on;
  Serial.printf("sys.channel_single:%d\r\n",sys.channel_single); 
  return AT_OK;  
}

/**************       AT_CHS       **************/
ATEerror_t at_CHS_get(const char *param)
{
  if(keep)
    Serial.print(AT CHS "=");
  Serial.println(sys.fre);  
}
ATEerror_t at_CHS_set(const char *param)
{
  uint32_t is_on = atoi(param); 
  if(strlen(param)!=1)
    return AT_PARAM_ERROR;   
  sys.fre = is_on;
  sys.customize_freq_set();
}
/**************       AT_PDOP       **************/
ATEerror_t at_PDOP_get(const char *param)
{
  if(keep)
    Serial.print(AT PDOP "=");
  Serial.println(sys.pdop_value);
  return AT_OK;
}
ATEerror_t at_PDOP_set(const char *param)
{
  float pdop_value = atoi(param);
  
  if(pdop_value > 7)
  {
    return AT_PARAM_ERROR;
  }
  sys.pdop_value = pdop_value;
  return AT_OK;  
}

/**************       AT_DWELLT       **************/
ATEerror_t at_dwelltime_get(const char *param)
{
  if(keep)
    Serial.print(AT DWELLT "=");
  Serial.println(sys.Dwelltime);
  return AT_OK;
}
ATEerror_t at_dwelltime_set(const char *param)
{
  uint8_t is_on = atoi(param);
  
  if(is_on >=2)
  {
    return AT_PARAM_ERROR;
  }
  sys.Dwelltime = is_on;
  return AT_OK;
}

/**************       AT_BLEMASK       **************/
ATEerror_t at_blemask_get(const char *param)
{
 if(keep)
  Serial.print(AT BLEMASK "=");
 if(strlen(sys.blemask_data)<=10)
 {
  for(int i=0;i<strlen(sys.blemask_data);i++)
  {
     Serial.printf("%c",sys.blemask_data[i]);
  }
  Serial.println();  
 }
 return AT_OK;
}
ATEerror_t at_blemask_set(const char *param)
{
  int i = strlen(param);
  if(i <= 10)
  {
    for(int a=0,b=0;a<strlen(param);a++)
    {
      sys.blemask_data[b++] = *(param+a);
    }
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  return AT_OK;
}

/**************       AT_WiFiMASK       **************/
ATEerror_t at_wifimask_get(const char *param)
{
 if(keep)
  Serial.print(AT WiFiMASK "=");
 if(strlen(sys.wifimask_data)<=10)
 {
  for(int i=0;i<strlen(sys.wifimask_data);i++)
  {
     Serial.printf("%c",sys.wifimask_data[i]);
  }
  Serial.println();  
 }
 return AT_OK;
}
ATEerror_t at_wifimask_set(const char *param)
{
  int i = strlen(param);
  if(i <= 10)
  {
    for(int a=0,b=0;a<strlen(param);a++)
    {
      sys.wifimask_data[b++] = *(param+a);
    }
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  return AT_OK;
}

/**************       AT_SHOWID       **************/
ATEerror_t at_showid_get(const char *param)
{
  if(keep)
    Serial.print(AT SHOWID  "=");
  Serial.println(sys.showid);
  return AT_OK;
}
ATEerror_t at_showid_set(const char *param)
{
 uint8_t SHOWid = atoi(param);
  if(SHOWid > 2)
  {
    return AT_PARAM_ERROR;
  }
  sys.showid = SHOWid;
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

uint8_t StrToHex(uint8_t *pbDest, char *pbSrc, uint8_t nLen)
{
  char h1,h2;
  uint8_t s1,s2;
  uint8_t i=0;

  for (; i<nLen; i++)
  {
    h1 = pbSrc[2*i];
    h2 = pbSrc[2*i+1];

    s1 = toupper(h1) - 0x30;
    if (s1 > 9) 
      s1 -= 7;

    s2 = toupper(h2) - 0x30;
    if (s2 > 9) 
      s2 -= 7;

    pbDest[i] = s1*16 + s2;
  }
  
  return i;
}
