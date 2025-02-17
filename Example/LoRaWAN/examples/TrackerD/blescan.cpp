/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
   Changed to a beacon scanner to report iBeacon, EddystoneURL and EddystoneTLM beacons by beegee-tokyo
*/
#include "blescan.h"
#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>
#include <BLEClient.h>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

int scanTime = 5; //In seconds
BLEScan *pBLEScan;
char brssi[10],bmin[10],bmax[10];
int sc_count =0;
int sc_flag =0,num = 0;
int mod =0;
char bufftest[100];
char bufftest1[100];
uint8_t mydata1[5] = {0};
uint8_t bledata1[240]= {0};
BLEBeacon id;

String reverse(String str) {
  String rev;
  for (int i = str.length() - 1; i >= 0; i--) {
    rev = rev + str[i];
  } 
  return rev;

}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {

        if (advertisedDevice.haveManufacturerData() == true)
        {
          std::string strManufacturerData = advertisedDevice.getManufacturerData();
          int dataLength = advertisedDevice.getManufacturerData().length();
          char *pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)advertisedDevice.getManufacturerData().data(), dataLength);    
          char cManufacturerData[100];
          char Address[12];
          strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);      
          if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00 && strlen(pHex) == 50)
          {
              if(sys.lon == 1)
              {
                digitalWrite(2, HIGH);
              }
              BLEBeacon oBeacon = BLEBeacon();
              oBeacon.setData(strManufacturerData);
              Serial.println("Found an iBeacon!");
              id.setData(advertisedDevice.getManufacturerData());
                                  
              //Print UUID
  //            Serial.print("UUID :");
              String bUUID = id.getProximityUUID().toString().c_str();
              bUUID = reverse(bUUID);
//              Serial.print(bUUID);
              String getAddress = advertisedDevice.getAddress().toString().c_str(); 
              int a=0;
              for(int i=0;i<getAddress.length();i++)
              {
                if(getAddress[i] !=':')
                {
                Address[a++]=getAddress[i];
                }
              }
              Address[a++]='\0'; 
             if(sys.showid == 1&& sys.ble_mod == 4)
             {                            
              for(int i=0;i<12;i++)
                  Serial.printf("%c",Address[i]);
              Serial.println();
             }               
                                
              //Print RSSI
  //            Serial.print(",RSSI :");
              int bRSSI = advertisedDevice.getRSSI();
//              Serial.print(bRSSI);
              ltoa(bRSSI,brssi,10);
  //            Serial.printf(brssi);
              
              //Print Major
  //            Serial.print(",Major :");
              int bMajor = id.getMajor() / 256;
  //            Serial.print(bMajor);
        
              //Print Minor
  //            Serial.print(",Minor :");
              int bMinor = id.getMinor() / 256;
  //            Serial.print(bMinor);
              Serial.println("");
              memset(mydata1,0,sizeof(mydata1));
              memcpy(bufftest1,pHex+40,strlen(pHex)-42);
              int i=0,j=0;
              for(;i<(strlen(bufftest1)/4)+2;i++,j++)
              {
                char str[5] = {0};
                memcpy(str,bufftest1+i*4,4);
                int num = hexToint_ble(str);
                mydata1[j*2] = num>>8;
                mydata1[j*2+1] = num; 
              }    
              for(i=4;i<5;i++)
              {
                 mydata1[i] = bRSSI;  
              }
             if(sys.showid == 1&& sys.ble_mod != 4)
             {                            
              for(int i=0;i<5;i++)
                  Serial.printf("%.2x ",mydata1[i]);
              Serial.println();
             }         
            int len =0;
            memset(bufftest,0,sizeof(bufftest));
            if(sys.ble_mod != 4)
            {           
              memcpy(bufftest,pHex+8,strlen(pHex)-8);
              strcat(bufftest,brssi); 
            }
            else
            {
              memcpy(bufftest,Address,strlen(Address));
              strcat(bufftest,brssi);               
            }
            len = strlen(sys.blemask_data);   
            if((strlen(sys.blemask_data) == 0 ||len <6 ||( sys.blemask_data[0] == '0' && sys.blemask_data[1] == '0' & sys.blemask_data[2] == '0' & sys.blemask_data[3] == '0' & sys.blemask_data[4] == '0' & sys.blemask_data[5] == '0'))&& bRSSI >= -90)
            {
               sc_count++;  
               if(sys.showid == 1 && sys.ble_mod != 3)
               {
                   Serial.print(AT BLEMASK "=");
                   if(strlen(sys.blemask_data)<=10)
                   {
                    for(int i=0;i<strlen(sys.blemask_data);i++)
                    {
                       Serial.printf("%c",sys.blemask_data[i]);
                    }
                    Serial.println();  
                   }
                  Serial.printf("bufftest:%s\r\n", bufftest);
                  Serial.printf("bufftest length:%d\r\n", strlen(bufftest));
               }
               strcat(sys.BLEDATA,bufftest);
              if(sys.ble_mod == 3)
              {
                if(sc_count<=40)
                {
                  for(int i=0;i<5;i++)
                  {
                    bledata1[i+sys.blecount] = mydata1[i];
//                    sys.blecount++;
                  }
//                  for(int i=0;i<sc_count*5;i++)
//                      Serial.printf("%.2x ",bledata1[i]);
//                  Serial.println();                  
                  sys.blecount = sc_count*5;
//                  Serial.printf("sys.blecount:%d\r\n", sys.blecount);                            
                } 
              }            
            }
            else if(strstr(bufftest,sys.blemask_data) != NULL && bRSSI >= -90)
            {
               sc_count++;  
               if(sys.showid == 1 && sys.ble_mod != 3)
               {
                   Serial.print(AT BLEMASK "=");
                   if(strlen(sys.blemask_data)<=10)
                   {
                    for(int i=0;i<strlen(sys.blemask_data);i++)
                    {
                       Serial.printf("%c",sys.blemask_data[i]);
                    }
                    Serial.println();  
                   }
                  Serial.printf("bufftest:%s\r\n", bufftest);
                  Serial.printf("bufftest length:%d\r\n", strlen(bufftest));
               }
               strcat(sys.BLEDATA,bufftest);            
              if(sys.ble_mod == 3)
              {
                if(sc_count <=40)
                {
                  for(int i=0;i<5;i++)
                  {
                    bledata1[i+sys.blecount] = mydata1[i];
//                    sys.blecount++;
                  }
                  sys.blecount = sc_count*5;
//                  Serial.printf("sys.blecount:%d\r\n", sys.blecount);                            
                } 
              }               
            }
          }
        }
        return;
    }
};
void ble_init()
{
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
  sc_count = 0;   
  pinMode(2, OUTPUT);
}

void ble_run()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(sys.ble_tdc, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");  
  ble_data();
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory 
}

//Bluetooth scan data processing
void ble_data(void)
{
  sys.exti_flag = 1;
  Serial.printf("sc_count= %d\r\n",sc_count);
  Serial.printf("DATA Length= %d\r\n",strlen(sys.BLEDATA));
  if(sys.exti_flag != 3 && sc_count != 0)
  {
//    for(int i = 0;i<strlen(sys.BLEDATA);i++)
//    {
//      Serial.printf("%c",sys.BLEDATA[i]);
//    }
//    Serial.println("");
    int len = strlen(sys.BLEDATA)/45;
//    if(len < 3 &&  sys.ble_mod != 3 )
//    {
//      sys.ble_mod = 1;
//    }
    if(sys.ble_mod == 0) //Find the three optimal beacons
    {
        int num2 = 0,num3 = 0;
        for(int a = 0;a<3;a++)     
        {
          strncpy(bmin,&sys.BLEDATA[42],3);   
//          Serial.printf("bmin:%s\r\n", bmin);
          for(int i =0;i<strlen(sys.BLEDATA)/45-1;i++)
          {
            strncpy(bmax,sys.BLEDATA+(45*(i+1)+42),3); 
//            Serial.printf("bmax:%s\r\n", bmax);
            if(strcmp(bmin,bmax)>0)
            {
              strncpy(bmin,bmax,3);
              if(a == 0)
              {
                num = i+1;
              }
              else if(a == 1)
              {
                num2 = i+1;
              }
              else if(a == 2)
              {
                num3 = i+1;                                               
              }
            }       
          }
          if(a == 0)
          {
            strncpy(sys.BLEDATA1,sys.BLEDATA+(num*45),45);
            sys.BLEDATA[(num*45)+42] = '9';
          }
          else if(a == 1)
          {
            strncpy(sys.BLEDATA2,sys.BLEDATA+(num2*45),45);
            sys.BLEDATA[(num2*45)+42] = '9';
          }
          else if(a == 2)
          {
            strncpy(sys.BLEDATA3,sys.BLEDATA+(num3*45),45);
            sys.BLEDATA[(num3*45)+42] = '9';
          }       
          sys.ble_flag = 1;                          
        }
        if(len == 2)
        {
         memset(sys.BLEDATA3,0,sizeof(sys.BLEDATA3));  
        }
        if(sys.showid == 1)
        {
          Serial.printf("DATA1:%s\r\n", sys.BLEDATA1);
          Serial.printf("DATA2:%s\r\n", sys.BLEDATA2);
          Serial.printf("DATA3:%s\r\n", sys.BLEDATA3);      
        }    
    }
    else if(sys.ble_mod == 1) //Find the optimal beacon
    {
      strncpy(bmin,&sys.BLEDATA[42],3);
      num = 0;   
      Serial.printf("bmin:%s\r\n", bmin);
      for(int i =0;i<strlen(sys.BLEDATA)/45-1;i++)
      {
        strncpy(bmax,sys.BLEDATA+(45*(i+1)+42),3); 
        Serial.printf("bmax:%s\r\n", bmax);
        if(strcmp(bmin,bmax)>0)
        {
          strncpy(bmin,bmax,3);
          num = i+1;
        }
      }
      sys.ble_flag = 1;
      memset(sys.BLEDATA1,0,sizeof(sys.BLEDATA1));
      strncpy(sys.BLEDATA1,sys.BLEDATA+(num*45),45);
      if(sys.showid == 1)
      {
        Serial.printf("BLEDATA1:%s\r\n", sys.BLEDATA1);   
      }
    }
    else if(sys.ble_mod == 3) //Scan all beacons
    {
      int i=0,j=0;
      for(int i=0;i<sys.blecount;i++)
      {
        sys.bledata[j++]=bledata1[i];
      }
      if(sys.showid == 1)
      {
        for(int i=0;i<sys.blecount;i++)
              Serial.printf("%.2x ",sys.bledata[i]);
          Serial.println();   
      }
      sys.ble_flag = 1;
    } 
    else if(sys.ble_mod == 4) //Find the three optimal beacons
    {
        int num2 = 0,num3 = 0;
        for(int a = 0;a<3;a++)     
        {
          strncpy(bmin,&sys.BLEDATA[12],3);   
//          Serial.printf("bmin:%s\r\n", bmin);
          for(int i =0;i<strlen(sys.BLEDATA)/15-1;i++)
          {
            strncpy(bmax,sys.BLEDATA+(15*(i+1)+12),3); 
//            Serial.printf("bmax:%s\r\n", bmax);
            if(strcmp(bmin,bmax)>0)
            {
              strncpy(bmin,bmax,3);
              if(a == 0)
              {
                num = i+1;
              }
              else if(a == 1)
              {
                num2 = i+1;
              }
              else if(a == 2)
              {
                num3 = i+1;                                               
              }
            }       
          }
          if(a == 0)
          {
            strncpy(sys.BLEDATA1,sys.BLEDATA+(num*15),15);
            if(sys.showid == 1)
            {
            Serial.printf("DATA1:%s\r\n", sys.BLEDATA1); 
            }            
            sys.BLEDATA[(num*15)+12] = '9';
          }
          else if(a == 1)
          {
            strncpy(sys.BLEDATA2,sys.BLEDATA+(num2*15),15);
            if(sys.showid == 1)
            {
            Serial.printf("DATA2:%s\r\n", sys.BLEDATA2); 
            }            
            sys.BLEDATA[(num2*15)+12] = '9';
          }
          else if(a == 2)
          {
            strncpy(sys.BLEDATA3,sys.BLEDATA+(num3*15),15);
            strncpy(sys.BLEDATA4,sys.BLEDATA3,15);
            if(sys.showid == 1)
            {
            Serial.printf("DATA3:%s\r\n", sys.BLEDATA3); 
            } 
            sys.BLEDATA[(num3*15)+12] = '9';
          }       
//          sys.ble_flag = 1;                          
        }
//        if(len == 2)
//        {
//         memset(sys.BLEDATA3,0,sizeof(sys.BLEDATA3));  
//        }
//        sys.ble_flag = 1;
       if(strlen(sys.BLEDATA) ==15)
        {
          for(int i=0;i<15;i++)
          {
            sys.BLEDATA2[i]= {'f'};
          }
          for(int i=0;i<15;i++)
          {
            sys.BLEDATA4[i]= {'f'};
          }        
          sys.ble_flag = 1;   
        } 
       else if(strlen(sys.BLEDATA)==30)
        {
          for(int i=0;i<15;i++)
          {
            sys.BLEDATA4[i]= {'f'};
          }        
          sys.ble_flag = 1;   
        }
        else if(strlen(sys.BLEDATA)==45)
        {
          sys.ble_flag = 1;    
        }          
    }       
  }
  else if(sys.sensor_mode == 2 && sc_count == 0 )
  {
     if(sys.ble_mod == 4 && sc_count == 0 )
      {
        for(int i=0;i<15;i++)
        {
          sys.BLEDATA1[i]= {'f'};
        }
        for(int i=0;i<15;i++)
        {
          sys.BLEDATA2[i]= {'f'};
        }
        for(int i=0;i<15;i++)
        {
          sys.BLEDATA4[i]= {'f'};
        }        
        sys.ble_flag = 1;   
      }
     else
     {
        for(int i=0;i<45;i++)
        {
          sys.BLEDATA1[i]= {'f'};
        }
        sys.ble_flag = 1;                
     }
 
  }
 else if((strstr(bufftest,sys.blemask_data) == NULL && sc_count == 0)||(sys.sensor_mode == 3 && sc_count == 0))
 {
    if(sys.sensor_mode == 3)
    {
      sys.ble_flag = 2;
    }
 }
 digitalWrite(2, LOW);
}

int hexToint_ble(char *str)
{
  int i;
  int n = 0;
  if(str[0] == '0' && (str[1]=='x' || str[1]=='X'))
    i = 2;
  else
    i = 0;
  for(;(str[i]>='0' && str[i]<='9') || (str[i] >='a' && str[i]<='z') || (str[i]>='A' && str[i]<='Z');i++)
  {
    if(tolower(str[i]) > '9')
     n= 16*n+(10+tolower(str[i]) - 'a');
    else
     n= 16*n + (tolower(str[i]) -'0');
  }
  return n;
}
