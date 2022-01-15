#include "GPS.h"
#include <TinyGPS++.h>
#include <HardwareSerial.h>

HardwareSerial SerialGPS(1);
TinyGPSPlus gps;
float latitude , longitude;
String  latitude_string , longitiude_string;
#define GPS_POWER 12

void GPS_INIT()
{
  pinMode(GPS_POWER,OUTPUT);
  digitalWrite(GPS_POWER, HIGH);  
  Serial.println("Connecting to ");
  SerialGPS.begin(9600, SERIAL_8N1, 9, 10);
  Serial.println("Minitor Dragino LoRa GPS Shield Status");
}

void GPS_sleep()
{
  pinMode(GPS_POWER,OUTPUT);
  digitalWrite(GPS_POWER, LOW);  
}
void GPS_DATA()
{
  
//  digitalWrite(LED_PIN_GREEN, HIGH);  
//  delay(1000);
//  digitalWrite(LED_PIN_GREEN, LOW);  
  while (SerialGPS.available() > 0) {
      if (gps.encode(SerialGPS.read()))
      {
        if (gps.location.isValid())
        {
          latitude = gps.location.lat();
          latitude_string = String(latitude , 6);
          longitude = gps.location.lng();
          longitiude_string = String(longitude , 6);
          Serial.print("Latitude = ");
          Serial.println(latitude_string);
          Serial.print("Longitude = ");
          Serial.println(longitiude_string);
        }
       delay(1000);
       Serial.println();  
      }
    }  
}
