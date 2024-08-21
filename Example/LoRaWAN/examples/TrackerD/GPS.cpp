#include "GPS.h"
#include <TinyGPS++.h>

HardwareSerial SerialGPS(1);
TinyGPSPlus gps;
unsigned long last = 0UL;
int End_times = 30;

String gogpddata="";


TinyGPSCustom model(gps, "GNGSA", 2);  // $GPGSA/GNGSA sentence, 2th element
TinyGPSCustom utc_time(gps, "$GPRMC", 1);  // $GPRMC/GNRMC sentence, 2th element
//TinyGPSCustom pdop(gps, "GNGSA", 15); // $GPGSA/GNGSA sentence, 15th element
//TinyGPSCustom hdop(gps, "GNGSA", 16); // $GPGSA/GNGSA sentence, 16th element
//TinyGPSCustom vdop(gps, "GNGSA", 17); // $GPGSA/GNGSA sentence, 17th element

void GPS_Init(uint8_t power_pin)
{
  pinMode(power_pin,OUTPUT);
  pinMode(GPS_RESET,OUTPUT);
//  SYS();
  SerialGPS.begin(9600, SERIAL_8N1, 9, 10);
}

void GPS_DeInit(void)
{
  SerialGPS.flush();
  SerialGPS.end();
}

void GPS_boot(void)
{
  digitalWrite(GPS_POWER, HIGH);  
  digitalWrite(GPS_RESET, LOW);
}

void GPS_shutdown(void)
{
  digitalWrite(GPS_POWER, LOW); 
//  digitalWrite(GPS_RESET, LOW); 
  GPS_DeInit(); 
}

bool GPS_DATA(void)
{
  while (SerialGPS.available() > 0)
  {
    if (gps.encode(SerialGPS.read()))
    {
//    Every time anything is updated, print everything.
      if (gps.altitude.isUpdated() || gps.satellites.isUpdated() || gps.hdop.isUpdated()&& model.isUpdated()
      || gps.location.isValid() || gps.date.isUpdated() || gps.time.isUpdated() && utc_time.isUpdated())
      {
        String mode1;
        mode1 =  model.value();
        sensor.pdop_gps = gps.hdop.hdop();
        sensor.Fix_Status = mode1.toInt();
        float latitude,longitude;
//        Serial.print(" HDOP = ");
//        Serial.print(sensor.pdop_gps); 
//        Serial.print(F(" UTC=")); Serial.print(utc_time.value()); 
//        Serial.print(F(" MODE=")); Serial.println(sensor.Fix_Status);
//        Serial.print("Latitude = ");
//        Serial.print(gps.location.lat(), 6);
//        Serial.print("Longitude = ");
//        Serial.println(gps.location.lng(), 6);

        sensor.year_gps = gps.date.year();
        sensor.month_gps = gps.date.month();
        sensor.day_gps = gps.date.day();

        sensor.hour_gps = gps.time.hour();
        sensor.minute_gps = gps.time.minute();
        sensor.second_gps = gps.time.second();  
        
        if(sys.showid == 1)
        {
          latitude = gps.location.lat();
          longitude = gps.location.lng();
          Serial.printf("Latitude = %0.6f\n\r",latitude);
          Serial.printf("Longitude = %0.6f\n\r",longitude);
          Serial.printf("Date: %d-%d-%d\n\r",sensor.year_gps,sensor.month_gps,sensor.day_gps);
          Serial.printf("Time: %d:%d:%d\n\r",(sensor.hour_gps<16)?sensor.hour_gps+8:sensor.hour_gps-16,sensor.minute_gps,sensor.second_gps);
          Serial.printf("PDOP = %0.2f\n\r",sensor.pdop_gps);
          Serial.printf("Fix Status = %d\n\r",sensor.Fix_Status);
          
        }         
        if((sensor.year_gps!=0)&&(sensor.month_gps!=0)&&(sensor.day_gps != 2000))
        {
          if((sys.pdop_value>=sensor.pdop_gps)&&(sensor.pdop_gps!=0.0)&&(sensor.Fix_Status == 3))
  //        if((sys.pdop_value>=sensor.pdop_gps)&&(sensor.pdop_gps!=0.0))
          {
            String  latitude_string , longitiude_string,year_string,month_string,day_string,hour_string,minute_string,second_string;
            latitude = gps.location.lat();
            sensor.latitude = (int)(latitude*1000000);
            latitude_string = String(sensor.latitude);
    
            longitude = gps.location.lng();
            sensor.longitude = (int)(longitude*1000000);
            longitiude_string = String(sensor.longitude);

	    //
	    // Altitude and HDOP for ttnmapper mode
	    sensor.altitude = gps.altitude.meters();
	    sensor.hdop_gps = (int)sensor.pdop_gps * 10;
	    
            Serial.printf("Latitude = %0.6f\n\r",latitude);
            Serial.printf("Longitude = %0.6f\n\r",longitude);
            Serial.printf("Date: %d-%d-%d\n\r",sensor.year_gps,sensor.month_gps,sensor.day_gps);
            Serial.printf("Time: %d:%d:%d\n\r",(sensor.hour_gps<16)?sensor.hour_gps+8:sensor.hour_gps-16,sensor.minute_gps,sensor.second_gps);
            Serial.printf("Fix Time:%dms\r\n",millis() - last);
            int i = 0;
            sys.gps_data_buff[i++] = (sensor.latitude>>24)  & 0xFF;
            sys.gps_data_buff[i++] = (sensor.latitude>>16)  & 0xFF;
            sys.gps_data_buff[i++] = (sensor.latitude>>8)   & 0xFF;
            sys.gps_data_buff[i++] = (sensor.latitude)      & 0xFF;
            sys.gps_data_buff[i++] = (sensor.longitude>>24) & 0xFF;
            sys.gps_data_buff[i++] = (sensor.longitude>>16) & 0xFF;
            sys.gps_data_buff[i++] = (sensor.longitude>>8)  & 0xFF;
            sys.gps_data_buff[i++] = (sensor.longitude)     & 0xFF;
            sys.gps_data_buff[i++] = (sensor.year_gps>>8)   & 0xFF;
            sys.gps_data_buff[i++] = (sensor.year_gps)      & 0xFF;
            sys.gps_data_buff[i++] = (sensor.month_gps)     & 0xFF;
            sys.gps_data_buff[i++] = (sensor.day_gps)       & 0xFF;
            sys.gps_data_buff[i++] = (sensor.hour_gps)      & 0xFF;
            sys.gps_data_buff[i++] = (sensor.minute_gps)    & 0xFF;
            sys.gps_data_buff[i++] = (sensor.second_gps)    & 0xFF; 
            if(sys.sensor_type == 22)  
            {
              sensor.bat = BatGet(); 
              sys.gps_data_buff[i++] = (sensor.bat>>8)        & 0xFF;
              sys.gps_data_buff[i++] = (sensor.bat)           & 0xFF;  
            }             

//            sys.gps_data_Weite();
//            Serial.printf("addr_gps_write:%d\r\n",sys.addr_gps_write);    
//            sys.read_gps_data_on_flash();
//            Serial.printf("addr_gps_read:%d\r\n",sys.addr_gps_read);       
            if(sys.lon == 1)
            {
              if(sys.sensor_type == 22)  
              {              
                digitalWrite(LED_PIN_BLUE1, HIGH);
                delay(1000); 
                digitalWrite(LED_PIN_BLUE1, LOW);
              }
              else
              {
                digitalWrite(LED_PIN_BLUE, HIGH);
                delay(1000); 
                digitalWrite(LED_PIN_BLUE, LOW);                
              }
            }
            return true;
          }
          else
          {
            if(sys.lon == 1)
            {
              if(sys.sensor_type == 22)  
              {              
                digitalWrite(LED_PIN_BLUE1, HIGH);
                delay(100); 
                digitalWrite(LED_PIN_BLUE1, LOW);
                delay(100);
              }
              else
              {
                digitalWrite(LED_PIN_BLUE, HIGH);
                 delay(100); 
                digitalWrite(LED_PIN_BLUE, LOW);   
                 delay(100);             
              }
            }        
            if (millis() - last > 30000)
            {
              last = millis();
              Serial.printf("Fix Time:%ds\r\n",End_times);
              End_times = End_times+30;
            }  
            return false;            
          }
        }
        else
        {  
          if(sys.lon == 1)
          {
              if(sys.sensor_type == 22)  
              {              
                digitalWrite(LED_PIN_BLUE1, HIGH);
                delay(100); 
                digitalWrite(LED_PIN_BLUE1, LOW);
                delay(100);
              }
              else
              {
                digitalWrite(LED_PIN_BLUE, HIGH);
                 delay(100); 
                digitalWrite(LED_PIN_BLUE, LOW);   
                 delay(100);             
              }
          }        
          if (millis() - last > 30000)
          {
            last = millis();
            Serial.printf("Fix Time:%ds\r\n",End_times);
            End_times = End_times+30;
          }               
          return false;
        }                      
      }
      else
      {
        return false;                           
      }
    }
    else
    {
      return false;                           
    }   
  }
  
}
