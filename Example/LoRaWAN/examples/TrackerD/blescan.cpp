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
          strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);      
          if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00 && strlen(pHex) == 50)
          {
            digitalWrite(2, HIGH);
            BLEBeacon oBeacon = BLEBeacon();
            oBeacon.setData(strManufacturerData);
            Serial.println("Found an iBeacon!");
            id.setData(advertisedDevice.getManufacturerData());
            //Print UUID
//            Serial.print("UUID :");
            String bUUID = id.getProximityUUID().toString().c_str();
            bUUID = reverse(bUUID);
//            Serial.print(bUUID);
      
            //Print RSSI
//            Serial.print(",RSSI :");
            int bRSSI = advertisedDevice.getRSSI();
//            Serial.print(bRSSI);
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
//            Serial.println("");
            memset(bufftest,0,sizeof(bufftest));
            memcpy(bufftest,pHex+8,strlen(pHex)-8);
            strcat(bufftest,brssi);         
            //如果找到的UUID相同，且RSSI大於-80時，亮燈
            if(strstr(bufftest,sys.blemask_data) != NULL && bRSSI >= -90)
            {
              sc_count++;  
              sys.exti_flag = 0;
              Serial.printf("bufftest:%s\r\n", bufftest);
              Serial.printf("bufftest length:%d\r\n", strlen(bufftest));
              strcat(sys.BLEDATA,bufftest);
              if(sc_count == 1)
              {
                memcpy(sys.BLEDATA1,pHex+8,strlen(pHex)-8); 
                strcat(sys.BLEDATA1,brssi);
                Serial.printf("BLEDATA1:%s\r\n", sys.BLEDATA1);
              }
              if(sc_count == 2)
              {
                memcpy(sys.BLEDATA2,pHex+8,strlen(pHex)-8);
                strcat(sys.BLEDATA2,brssi);
                Serial.printf("BLEDATA2:%s\r\n", sys.BLEDATA2);
              }
              if(sc_count == 3)
              {
                memcpy(sys.BLEDATA3,pHex+8,strlen(pHex)-8); 
                strcat(sys.BLEDATA3,brssi);
                Serial.printf("BLEDATA3:%s\r\n", sys.BLEDATA3);
              }
              if(sc_count == 4)
              {
                memcpy(sys.BLEDATA4,pHex+8,strlen(pHex)-8); 
                strcat(sys.BLEDATA4,brssi);
                Serial.printf("BLEDATA4:%s\r\n", sys.BLEDATA4);
              }
              if(sc_count == 5)
              {
                memcpy(sys.BLEDATA5,pHex+8,strlen(pHex)-8); 
                strcat(sys.BLEDATA5,brssi);
                Serial.printf("BLEDATA5:%s\r\n", sys.BLEDATA5);
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
  pinMode(2, OUTPUT);
}

void ble_run()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");  
  ble_data();
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory 
}

//Bluetooth scan data processing
void ble_data(void)
{
  if(sys.exti_flag != 3 && sc_count != 0)
  {
    Serial.printf("DATA Length= %d\r\n",strlen(sys.BLEDATA));
    for(int i = 0;i<strlen(sys.BLEDATA);i++)
    {
      Serial.printf("%c",sys.BLEDATA[i]);
      sys.mod = 3;
    }
    Serial.println("");
    int len = strlen(sys.BLEDATA)/45;
    if(strlen(sys.BLEDATA) < 135)
    {
      sys.mod = 3;
    }
    if(sys.mod == 2) //Find the three optimal beacons
    {
        int num2 = 0,num3 = 0;
        for(int a = 0;a<3;a++)     
        {
          strncpy(bmin,&sys.BLEDATA[42],3);   
          Serial.printf("bmin:%s\r\n", bmin);
          for(int i =0;i<strlen(sys.BLEDATA)/45-1;i++)
          {
            strncpy(bmax,sys.BLEDATA+(45*(i+1)+42),3); 
            Serial.printf("bmax:%s\r\n", bmax);
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
        Serial.printf("DATA1:%s\r\n", sys.BLEDATA1);
        Serial.printf("DATA2:%s\r\n", sys.BLEDATA2);
        Serial.printf("DATA3:%s\r\n", sys.BLEDATA3);          
    }
    else if(sys.mod == 3) //Find the optimal beacon
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
      strncpy(sys.BLEDATA1,sys.BLEDATA+(num*45),45);
      Serial.printf("BLEDATA1:%s\r\n", sys.BLEDATA1);   
    } 
    sc_count = 0;   
  }
  else if(sys.sensor_mode == 2 && sc_count == 0)
  {
    sys.ble_flag = 1;
    for(int i=0;i<45;i++)
    {
      sys.BLEDATA1[i]= {'f'};
    }   
  }
  else if(strstr(bufftest,sys.blemask_data) == NULL && sc_count == 0)
  {
    if(sys.sensor_mode == 3)
    {
      sys.ble_flag = 2;
    }
  }   
  digitalWrite(2, LOW);
}
