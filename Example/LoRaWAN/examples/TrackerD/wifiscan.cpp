#include "wifiscan.h"
#include "WiFi.h"
char buff_test[10],WIFIData[500],Cssid[12],WDATA[500];
char *addSSID="cccccc";
char wmin[10],wmax[10];
char WRSSI[10];
char WSSID[10];
char buff[10];
int num1 = 0;
int wifi_count = 0, wrssi = 0;
int chinese_flag = 0;



void wifi_scan(void)
{
  if(sys.lon == 1)
  {
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
  }
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Scanning...");
  memset(WIFIData,0,sizeof(WIFIData));
//  WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) 
  {
  for(int i=0;i<9;i++)
  {
    sys.databuf[i]= {'f'};
  }
  sys.ble_flag = 1;   
  digitalWrite(2, LOW);     
  Serial.println("no networks found");
  }
  else 
  {
    Serial.print(n);
    Serial.println(" networks found");
    wifi_count = 0; 
    for (int i = 0; i < n; i++) 
    {
//        Print SSID and RSSI for each network found
      if(sys.showid == 1)
      {
        Serial.print(i + 1);              
        Serial.print("WIFI: ");
        Serial.print(WiFi.SSID(i));
        Serial.print(": ");
        Serial.println(WiFi.RSSI(i)); 
      }
      memset(buff_test,0,sizeof(buff_test));           
      memset(WSSID,0,sizeof(WSSID));
      memset(WRSSI,0,sizeof(WRSSI));
      String ssid = WiFi.SSID(i);
      const char *str = ssid.c_str();
      chinese_flag = 0;
      for(int i =0;i < strlen(str);i++)
      {
        if(*(str+i)& 0x80)
        {
          chinese_flag = 1;
        }
      }
      if(chinese_flag == 0)
      {
        if(strstr(str,sys.wifimask_data) != NULL)
        {
          if(strlen(str)>6)
          {
            strncpy(WSSID,str+(strlen(str)-6),6);
//            Serial.printf("%s\r\n", WSSID);
          }
          if(strlen(str)<=6)
          {
            strncpy(Cssid,str,6);
            strcat(Cssid,addSSID);
            strncpy(WSSID,Cssid,6);
//            Serial.printf("%s\r\n", WSSID);
          }
          strcat(buff_test,WSSID);          
          wrssi = WiFi.RSSI(i);
          ltoa(wrssi,WRSSI,10);
          strcat(buff_test,WRSSI);
          strcat(WIFIData,buff_test);
          wifi_count = 1;
          sys.exti_flag=1;
//          Serial.printf("sys.ble_mod:%d\r\n", sys.ble_mod);
//          Serial.printf("wifi_count:%d\r\n", wifi_count);
//          Serial.printf("sys.exti_flag:%d\r\n", sys.exti_flag);
        }
        else if((strlen(sys.wifimask_data)<6 ||( sys.wifimask_data[0] == '0' && sys.wifimask_data[1] == '0' & sys.wifimask_data[2] == '0' & sys.wifimask_data[3] == '0' & sys.wifimask_data[4] == '0' & sys.wifimask_data[5] == '0'))&& wrssi >= -90)
        {
          memset(sys.databuf,0,sizeof(sys.databuf));
          memset(WSSID,0,sizeof(WSSID));
          memset(WRSSI,0,sizeof(WRSSI));
          String ssid = WiFi.SSID(0);
          if(sys.showid == 1)
          {
            Serial.print("WIFI: ");
            Serial.print(WiFi.SSID(0));
            Serial.print(": ");
            Serial.println(WiFi.RSSI(0)); 
          }   
          const char *str = ssid.c_str();
          if(strlen(str)>6)
          {
            strncpy(WSSID,str+(strlen(str)-6),6);
//            Serial.printf("%s\r\n",WSSID);
          }
          if(strlen(str)<=6)
          {
            strncpy(Cssid,str,6);
            strcat(Cssid,addSSID);
            strncpy(WSSID,Cssid,6);
//            Serial.printf("%s\r\n", WSSID);
          }           
          strcat(sys.databuf,WSSID);
          int wrssi = WiFi.RSSI(0);
          ltoa(wrssi,WRSSI,10);
          strcat(sys.databuf,WRSSI);  
          sys.ble_flag = 1;
          digitalWrite(2, LOW); 
          break;              
        }
        else if(strlen(WIFIData) == 0 && sys.sensor_mode == 2)
        {
          for(int i=0;i<9;i++)
          {
           sys.databuf[i]= {'f'};
          };
          sys.ble_flag = 1; 
          digitalWrite(2, LOW);     
        }
        else if(strlen(WIFIData) == 0 && sys.sensor_mode == 3)
        {
          sys.ble_flag = 2; 
          digitalWrite(2, LOW);     
        }   
      }
    } 
  }
  if(sys.ble_mod == 2 && wifi_count == 1 && sys.exti_flag != 3)
  {
//    Serial.printf("WIFIData1:%s\r\n", WIFIData);  
    strncpy(wmin,&WIFIData[6],3);
    num1 = 0;
    for(int a =0;a<strlen(WIFIData)/9-1;a++)
    {
      strncpy(wmax,WIFIData+(9+(a+1)+75),3);
      if(strcmp(wmin,wmax)>0)
      {
      strncpy(wmin,wmax,3);
      num1 = a+1;
      }
    }
    strncpy(sys.databuf,WIFIData+(num1*9),9);
    memset(WIFIData,0,sizeof(WIFIData));
    digitalWrite(2, LOW);   
    sys.ble_flag = 1;
//    Serial.printf("databuf:%s\r\n", sys.databuf);   
  }  
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}
