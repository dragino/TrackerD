#include <SPI.h>
#include "common.h"
#include "lis3dh-motion-detection.h"
#include "driver/rtc_io.h" 
#include "hal/rtc_io_hal.h"
#define HIGH_RESOLUTION
#define maxRxSize       128 
#define EXTI_PIN        0
#define BUTTON_PIN_BITMASK 0x000000001

LIS3DH myIMU(0x19); //Default address is 0x19.
uint16_t sampleRate = 1;  // HZ - Samples per second - 1, 10, 25, 50, 100, 200, 400, 1600, 5000
uint8_t accelRange = 2;   // Accelerometer range = 2, 4, 8, 16g

static bool send_complete = false;
static bool os_run_flag = false;
int os_JOINED_flag = 0;
int fcnt_flag = 0;
int turn_interrupts = 0;
int interrupts_count = 0;
int interrupts_flag = 0;
int EXIT_flag = 0;
int intwk_flag =0;
unsigned long buzzerlast = 0UL;
unsigned long Exit_Alarm = 0UL;

unsigned long sleep_last = 0UL;
static char rxData[maxRxSize]={0};
hw_timer_t * timer = NULL;
hw_timer_t * timer1 = NULL;
volatile SemaphoreHandle_t timerSemaphore;
volatile SemaphoreHandle_t timerSemaphore1;
// Saves the LMIC structure during DeepSleep
RTC_DATA_ATTR lmic_t RTC_LMIC;
RTC_DATA_ATTR uint32_t button_Count = 0;
RTC_DATA_ATTR int button_Count1 = 0;
RTC_DATA_ATTR int sport_Count = 0;
RTC_DATA_ATTR int sport_mod = 0;
RTC_DATA_ATTR int ack_datalog =0;
RTC_DATA_ATTR int TDC_Count =0;
RTC_DATA_ATTR int TDC_Time =0;
RTC_DATA_ATTR int Pedometer_Count =0;
RTC_DATA_ATTR int DFR =0;
RTC_DATA_ATTR int Pedometer_OFF =0;
RTC_DATA_ATTR int TDC_flag =0;
RTC_DATA_ATTR int Erase =0;
void os_getArtEui (u1_t* buf) { sys.LORA_GetAEUI(buf);}

void os_getDevEui (u1_t* buf) { sys.LORA_GetDEUI(buf);}

void os_getDevKey (u1_t* buf) { sys.LORA_GetAKEY(buf);}

static void LORA_RxData( uint8_t *AppData,uint8_t AppData_Len);
void gps_send(void);
void tdc_time(void);
void device_start(void);
void Alarm_start(void);
void sys_sleep(void);
void alarm_state(void);
int hexToint(char *str);
void LIS3DH_configIntterupts(void);

static uint8_t mydata[50] = {0};
static osjob_t sendjob;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 23,
    .dio = {26, 33, 32},
};

void ARDUINO_ISR_ATTR onTimer()
{
  // Give a semaphore that we can check in the loop
  //xSemaphoreGiveFromISR(timerSemaphore, NULL);
  sys.gps_work_flag = false;

}

void ARDUINO_ISR_ATTR keepTimer()
{
  // Give a semaphore that we can check in the loop
  //xSemaphoreGiveFromISR(timerSemaphore, NULL);
}

void LoraWANPrintLMICOpmode(void)
{
    Serial.print(F("LMIC.opmode: "));
    if (LMIC.opmode & OP_NONE)
    {
        Serial.print(F("OP_NONE "));
    }
    if (LMIC.opmode & OP_SCAN)
    {
        Serial.print(F("OP_SCAN "));
    }
    if (LMIC.opmode & OP_TRACK)
    {
        Serial.print(F("OP_TRACK "));
    }
    if (LMIC.opmode & OP_JOINING)
    {
        Serial.print(F("OP_JOINING "));
    }
    if (LMIC.opmode & OP_TXDATA)
    {
        Serial.print(F("OP_TXDATA "));
    }
    if (LMIC.opmode & OP_POLL)
    {
        Serial.print(F("OP_POLL "));
    }
    if (LMIC.opmode & OP_REJOIN)
    {
        Serial.print(F("OP_REJOIN "));
    }
    if (LMIC.opmode & OP_SHUTDOWN)
    {
        Serial.print(F("OP_SHUTDOWN "));
    }
    if (LMIC.opmode & OP_TXRXPEND)
    {
        Serial.print(F("OP_TXRXPEND "));
    }
    if (LMIC.opmode & OP_RNDTX)
    {
        Serial.print(F("OP_RNDTX "));
    }
    if (LMIC.opmode & OP_PINGINI)
    {
        Serial.print(F("OP_PINGINI "));
    }
    if (LMIC.opmode & OP_PINGABLE)
    {
        Serial.print(F("OP_PINGABLE "));
    }
    if (LMIC.opmode & OP_NEXTCHNL)
    {
        Serial.print(F("OP_NEXTCHNL "));
    }
    if (LMIC.opmode & OP_LINKDEAD)
    {
        Serial.print(F("OP_LINKDEAD "));
    }
    if (LMIC.opmode & OP_LINKDEAD)
    {
        Serial.print(F("OP_LINKDEAD "));
    }
    if (LMIC.opmode & OP_TESTMODE)
    {
        Serial.print(F("OP_TESTMODE "));
    }
    if (LMIC.opmode & OP_UNJOIN)
    {
        Serial.print(F("OP_UNJOIN "));
    } 
}

void onEvent (ev_t ev) 
{
  Serial.print(os_getTime());
  Serial.print(": ");
  switch(ev) {
      case EV_SCAN_TIMEOUT:
          Serial.println(F("EV_SCAN_TIMEOUT"));
          break;
      case EV_BEACON_FOUND:
          Serial.println(F("EV_BEACON_FOUND"));
          break;
      case EV_BEACON_MISSED:
          Serial.println(F("EV_BEACON_MISSED"));
          break;
      case EV_BEACON_TRACKED:
          Serial.println(F("EV_BEACON_TRACKED"));
          break;
      case EV_JOINING:
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_BLUE, HIGH);
            delay(200);
            digitalWrite(LED_PIN_BLUE, LOW);  
          }       
          Serial.println(F("EV_JOINING"));
          break;
      case EV_JOINED:
          Serial.println(F("EV_JOINED"));
          os_JOINED_flag = 1;
          fcnt_flag = 1;
          DFR =1;
          digitalWrite(LED_PIN_GREEN, HIGH);                   
          LMIC_getSessionKeys(&sys.netid, &sys.devaddr, sys.nwkSKey, sys.appSKey);
          // Disable link check validation (automatically enabled
          // during join, but because slow data rates change max TX
          // size, we don't use it in this example.
          LMIC_setLinkCheckMode(0);
          if(sys.LORA_GetADR() == ADR_ENABLE)
          {
            LMIC_setAdrMode(0);
            LMIC_setDrTxpow(sys.LORA_GetDR(),sys.LORA_GetTXP());
          }
          else
          {
            sys.LORA_SetDR(LMIC.datarate);
          } 
          delay(5000);             
          break;
      /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_RFU1:
      ||     Serial.println(F("EV_RFU1"));
      ||     break;
      */
      case EV_JOIN_FAILED:
          Serial.println(F("EV_JOIN_FAILED"));
          break;
      case EV_REJOIN_FAILED:
          Serial.println(F("EV_REJOIN_FAILED"));
          break;
      case EV_TXCOMPLETE: 
          Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
          if (LMIC.dataLen)
          {
            Serial.print(F("Received:"));
            for(uint8_t i=0;i<LMIC.dataLen;i++)
              Serial.printf("%.2X ",LMIC.frame[i+LMIC.dataBeg]);
            Serial.println();
            LORA_RxData(&LMIC.frame[LMIC.dataBeg],LMIC.dataLen);            
          }          
          if (LMIC.txrxFlags & TXRX_ACK)
          {
            if(sys.PNACKmd == 1)
            {
              sys.frame_ACK = 0;
              if(sys.gps_write == 4095)
              {
                if(sys.gps_write == sys.addr_gps_read)
                {
//                  sys.GPSDATA_CLEAR();
                  sys.gps_write  = 0;                  
                  sys.addr_gps_read = 0;
//                  sys.addr_gps_write = 0;
                  sport_mod =0;
                  sys.loggpsdata_send = 0;
                  if(sys.Intwk == 1 && fcnt_flag == 0 && (LMIC.seqnoUp-1 > 1))
                  {
                     button_Count1 = 1;
                     LIS3DH_configIntterupts();       
                  }
                  if(ack_datalog == 1) 
                  {
                   turn_interrupts = 0;
                   ack_datalog = 0;
                   button_Count1 = 0;
                  }
                }
              }
              else if(sys.addr_gps_write == sys.addr_gps_read)
              {
                sys.GPSDATA_CLEAR();
                sys.gps_write  = 0;
                sport_mod = 0; 
                sys.addr_gps_read = 0;
                sys.addr_gps_write = 0;
                sys.loggpsdata_send = 0;
                if(sys.Intwk == 1 && fcnt_flag == 0 && (LMIC.seqnoUp-1 > 1))
                {
                  button_Count1 = 1;
                  LIS3DH_configIntterupts();       
                }
                if(ack_datalog == 1) 
                {
                 turn_interrupts = 0;
                 ack_datalog = 0;
                 button_Count1 = 0;
                }                                 
              }
              if(sys.addr_gps_write >=14)
              {
                sys.loggpsdata_send = 1;
                if(sys.Intwk == 1 )
                {
                  sport_mod =1;
                  sys.alarm = 0;
                  ack_datalog = 1;
                  button_Count1 = 1;
                  turn_interrupts = 0;
                  sys.exti_flag  = 5;
                }
              }
            }
            if((LMIC.opmode & OP_NEXTCHNL)||(LMIC.opmode & OP_RNDTX))
            {
//              #if !defined(CFG_eu868)||!defined(CFG_kr920)|| !defined(CFG_in866)||!defined(CFG_au915)
                LMIC.opmode = OP_RNDTX ; 
                LMIC.opmode |= OP_NEXTCHNL ;           
//              #endif      
            } 
            Serial.println(F("Received ack"));           
            send_complete = true;
            break;
          }            
          else if (LMIC.txrxFlags & TXRX_NACK)
          {
            if(sys.PNACKmd == 1)
            {
             sport_mod = 0;
             if(sys.sensor_mode == 1 && sys.frame_flag == 1)
             {
              sys.config_Read();
              if(sys.addr_gps_write>0&&sys.addr_gps_write<14)
              {
                sys.GPSDATA_CLEAR();
                sys.addr_gps_write =0 ;
              }
              else if(sensor.latitude !=0 && sensor.longitude !=0)
              {
                sys.gps_data_Weite();
                sys.loggpsdata_send = 0;  
              }
              sys.loggpsdata_send = 0;
             } 
             if(sys.Intwk == 1 && fcnt_flag == 0 && (LMIC.seqnoUp-1 > 1) && button_Count1 == 0)
             {
                sport_mod =0;
                sys.alarm = 0;
                ack_datalog = 0;
                button_Count1 = 0;
                turn_interrupts = 0;
              }             
            }           
            sys.loggpsdata_send = 0;
            Serial.println(F("Received nack"));
            if((LMIC.opmode & OP_NEXTCHNL)||(LMIC.opmode & OP_RNDTX))
            {
  //              #if defined( CFG_as923 ) || defined( CFG_au915 ) 
                LMIC.opmode = OP_RNDTX ; 
                LMIC.opmode |= OP_NEXTCHNL ;
  //            sys_sleep();            
  //              #endif     
            }             
            send_complete = true;
            break;        
          }
          if((LMIC.opmode & OP_NEXTCHNL)||(LMIC.opmode & OP_RNDTX))
          {
//              #if defined( CFG_as923 ) || defined( CFG_au915 ) 
              LMIC.opmode = OP_RNDTX ; 
              LMIC.opmode |= OP_NEXTCHNL ;
//            sys_sleep();            
//              #endif     
          }           
          if(sys.addr_gps_read <= 0)
          {
           sys.addr_gps_read = 0;
          }
           send_complete = true;          
          break;
      case EV_LOST_TSYNC:
          Serial.println(F("EV_LOST_TSYNC"));
          break;
      case EV_RESET:
          Serial.println(F("EV_RESET"));
          break;
      case EV_RXCOMPLETE:
          // data received in ping slot
          Serial.println(F("EV_RXCOMPLETE"));
          break;
      case EV_LINK_DEAD:
          Serial.println(F("EV_LINK_DEAD"));
          break;
      case EV_LINK_ALIVE:
          Serial.println(F("EV_LINK_ALIVE"));
          break;
      /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_SCAN_FOUND:
      ||    Serial.println(F("EV_SCAN_FOUND"));
      ||    break;
      */
      case EV_TXSTART:
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_GREEN, HIGH);
            delay(200);
          }
          if(os_JOINED_flag == 0 || fcnt_flag == 1)
          { 
            if(fcnt_flag == 1)
            {
              button_Count1 = 0;
              Serial.println("UpLinkCounter = 0");
            }
            else
            { 
              Serial.printf("UpLinkCounter = %d\r\n",LMIC.seqnoUp-1);
            }
          }       
          if( (LMIC.txrxFlags & TXRX_DNW1) == 0 || (LMIC.txrxFlags & TXRX_NACK) !=0 || LMIC.dataLen == 0 )          
          {
            if(sys.PNACKmd == 1)
            {
             sys.frame_ACK = 1;
             LMIC.txCnt = 8; 
            }
          }               
          Serial.println(F("EV_TXSTART"));       
          break;
      case EV_TXCANCELED:
          Serial.println(F("EV_TXCANCELED"));
          break;
      case EV_RXSTART:
          /* do not print anything -- it wrecks timing */
          break;
      case EV_JOIN_TXCOMPLETE:
          Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
          break;

      default:
          Serial.print(F("Unknown event: "));
          Serial.println((unsigned) ev);
          break;
  }
  digitalWrite(LED_PIN_RED | LED_PIN_BLUE | LED_PIN_GREEN, LOW);
}


void do_send(osjob_t* j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
      Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
      int i = 0;
      // Prepare upstream data transmission at the next possible time.
      memset(mydata,0,50);    
//      if(sys.save_sensor_mode == 3)
//      {
//        if(sys.exti_flag == 4)
//        {
//          sys.mod = 0;
//          sys.sensor_mode = 1; 
//        }
//      }
      if(sys.loggpsdata_send == 1)
      {
        sys.port = 4;
        sys.read_gps_data_on_flash();
        Serial.printf("addr_gps_read:%d\r\n",sys.addr_gps_read);  
        mydata[i++] = sys.gps_data_buff[0] & 0xFF;
        mydata[i++] = sys.gps_data_buff[1] & 0xFF;
        mydata[i++] = sys.gps_data_buff[2] & 0xFF;
        mydata[i++] = sys.gps_data_buff[3] & 0xFF;
        mydata[i++] = sys.gps_data_buff[4] & 0xFF;
        mydata[i++] = sys.gps_data_buff[5] & 0xFF;
        mydata[i++] = sys.gps_data_buff[6] & 0xFF;
        mydata[i++] = sys.gps_data_buff[7] & 0xFF;   
        mydata[i++] = sys.gps_data_buff[8] & 0xFF;
        mydata[i++] = sys.gps_data_buff[9] & 0xFF;
        mydata[i++] = sys.gps_data_buff[10] & 0xFF;
        mydata[i++] = sys.gps_data_buff[11] & 0xFF;
        mydata[i++] = sys.gps_data_buff[12] & 0xFF;
        mydata[i++] = sys.gps_data_buff[13] & 0xFF;  
        mydata[i++] = sys.gps_data_buff[14] & 0xFF;              
      }
      else
      { 
        if(sensor.latitude == 0 && sensor.longitude == 0)
        {
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_RED, HIGH);
            delay(200); 
            digitalWrite(LED_PIN_RED, LOW);
          }      
        }
        if(sys.sensor_mode == 1 )
        { 
          mydata[i++] = (sensor.latitude>>24)  & 0xFF;
          mydata[i++] = (sensor.latitude>>16)  & 0xFF;
          mydata[i++] = (sensor.latitude>>8)   & 0xFF;
          mydata[i++] = (sensor.latitude)      & 0xFF;
          mydata[i++] = (sensor.longitude>>24) & 0xFF;
          mydata[i++] = (sensor.longitude>>16) & 0xFF;
          mydata[i++] = (sensor.longitude>>8)  & 0xFF;
          mydata[i++] = (sensor.longitude)     & 0xFF;   
          if(sys.mod == 0)
          {
            sys.port = 2;
            mydata[i++] = ((sys.alarm<<6) | (sensor.bat>>8)) & 0xFF;
            mydata[i++] = (sensor.bat) & 0xFF;
            mydata[i++] = ((sys.mod<<6) | (sys.lon<<5)|(intwk_flag<<4)) & 0xFF;   
            GXHT3x_GetData();
            sensor.hum = (int)(GXHT3x_GetHum()*10);
            sensor.tem = (int)(GXHT3x_GetTem()*10); 
            mydata[i++] = (sensor.hum>>8) & 0xFF;
            mydata[i++] = (sensor.hum)    & 0xFF;
            mydata[i++] = (sensor.tem>>8) & 0xFF;
            mydata[i++] = (sensor.tem)    & 0xFF;             
          }
          else if(sys.mod == 1)
          {
            sys.port = 3;
            mydata[i++] = ((sys.alarm<<6) | (sensor.bat>>8)) & 0xFF;
            mydata[i++] = (sensor.bat) & 0xFF;
            mydata[i++] = ((sys.mod<<6) | (sys.lon<<5)|(intwk_flag<<4)) & 0xFF;         
          }               
        }
        if(sys.sensor_mode == 2 || sys.sensor_mode == 3)
        {
          if(sys.ble_mod  == 0)
          {      
            ;                  
          }
          else if(sys.ble_mod  == 1)
          {
            sys.port = 6;
            char bleuuidmajorminjor[100]={0};
            char blerssi[4];
            for(int a=0;a<(strlen(sys.BLEDATA1))-3;a++)
            {
              bleuuidmajorminjor[a] = sys.BLEDATA1[a];
            }
            for(int a=42; a<strlen(sys.BLEDATA1); a++)
            {
              blerssi[a-42] = sys.BLEDATA1[a];
            }        
            for(int j=0;i<(strlen(bleuuidmajorminjor)/4)+13;i++,j++)
            {
              char str[5] = {0};
              memcpy(str,bleuuidmajorminjor+i*4,4);
              int num = hexToint(str);
              mydata[j*2] = num>>8;
              mydata[j*2+1] = num; 
            }
            String brssi = blerssi;
            int num = brssi.toInt();  
            mydata[i++] = num;     
            mydata[i++] = ((sys.alarm<<6) | (sensor.bat>>8)) & 0xFF;
            mydata[i++] = (sensor.bat) & 0xFF;
            mydata[i++] = ((sys.mod<<6) | (sys.lon<<5)|(intwk_flag<<4)) & 0xFF;  
            mydata[i++] = (button_Count>>24)  & 0xFF;
            mydata[i++] = (button_Count>>16)  & 0xFF;
            mydata[i++] = (button_Count>>8)   & 0xFF;
            mydata[i++] = (button_Count)      & 0xFF;                                 
          }
          else if(sys.ble_mod  == 2)
          {
            sys.port = 8;
            Serial.println("");
            char  Wssid[10]={0};
            char  Wrssi[10];
            for(int a=0;a<(strlen(sys.databuf))-3;a++)
            {
              Wssid[a] = sys.databuf[a];
            }
            for(int a=6; a<strlen(sys.databuf); a++)
            {
              Wrssi[a-6] = sys.databuf[a];
            } 
            String wrssi = Wrssi;
            int wifirssi = wrssi.toInt(); 
           for(int a=0;a<strlen(Wssid);a++)
            {
              mydata[i++] = Wssid[a];
            }
            mydata[i++] = wifirssi & 0xFF; 
            mydata[i++] = ((sys.alarm<<6) | (sensor.bat>>8)) & 0xFF;
            mydata[i++] = (sensor.bat) & 0xFF; 
            mydata[i++] = ((sys.mod<<6) | (sys.lon<<5)|(intwk_flag<<4)) & 0xFF;                                                 
          }
        }
      }        
      LMIC_setTxData2(sys.port, mydata, i, sys.frame_flag);
      Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.    
}
void device_send(osjob_t* j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
      Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
      int i = 0;
      // Prepare upstream data transmission at the next possible time.
      memset(mydata,0,50);
      Device_status();
      sys.port = 0x05;
      mydata[i++] = devicet.sensor_type;
//      if(sys.pedometer == 1)
//      { 
        mydata[i++] = (devicet.firm_ver>>24)&0xff;
        mydata[i++] = (devicet.firm_ver>>16)&0xff;
//      }
      mydata[i++] = (devicet.firm_ver>>8)&0xff;
      mydata[i++] = devicet.firm_ver&0xff;
      mydata[i++] = devicet.freq_band;
      mydata[i++] = devicet.sub_band;
      mydata[i++] = (devicet.battrey>>8)&0xff;
      mydata[i++] = devicet.battrey&0xff;
      mydata[i++] = devicet.SMODE ;
      mydata[i++] = devicet.FLAG ;      
      
      LMIC_setTxData2(sys.port, mydata, i,sys.frame_flag);
      Serial.println(F("Packet queued"));
    }

    // Next TX is scheduled after TX_COMPLETE event.        
}

void Alarm_send(osjob_t* j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
      Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
      int i = 0;
      // Prepare upstream data transmission at the next possible time.
      memset(mydata,0,50);
      Device_status();
      sys.port = 0x07;
      mydata[i++] = ((sys.alarm<<6) | (sensor.bat>>8)) & 0xFF;
      mydata[i++] = (sensor.bat) & 0xFF;
      mydata[i++] = ((sys.mod<<6) | (sys.lon<<5) | sys.frame) & 0xFF;   
      LMIC_setTxData2(sys.port, mydata, i,sys.frame_flag);
      Serial.println(F("Packet queued"));
    }
    
    // Next TX is scheduled after TX_COMPLETE event.        
}

void SaveLMICToRTC(int deepsleep_sec)
{
    //Serial.println(F("Save LMIC to RTC"));
    RTC_LMIC = LMIC;

    // ESP32 can't track millis during DeepSleep and no option to advanced millis after DeepSleep.
    // Therefore reset DutyCyles

    unsigned long now = millis();
#if CFG_LMIC_EU_like
    for (int i = 0; i < MAX_BANDS; i++)
    {
        ostime_t correctedAvail = RTC_LMIC.bands[i].avail - ((now / 1000.0 + deepsleep_sec) * OSTICKS_PER_SEC);
        if (correctedAvail < 0)
        {
            correctedAvail = 0;
        }
        RTC_LMIC.bands[i].avail = correctedAvail;
    }
    RTC_LMIC.globalDutyAvail = RTC_LMIC.globalDutyAvail - ((now / 1000.0 + deepsleep_sec) * OSTICKS_PER_SEC);
    if (RTC_LMIC.globalDutyAvail < 0)
    {
        RTC_LMIC.globalDutyAvail = 0;
    }
    
#else
//    for (int i = 0; i < 4; i++)
//    {
//        ostime_t correctedAvail = RTC_LMIC.channelDrMap[i].avail - ((now / 1000.0 + deepsleep_sec) * OSTICKS_PER_SEC);
//        if (correctedAvail < 0)
//        {
//            correctedAvail = 0;
//        }
//        RTC_LMIC.channelDrMap[i].avail = correctedAvail;
//    }

    RTC_LMIC.globalDutyAvail = RTC_LMIC.globalDutyAvail - ((now / 1000.0 + deepsleep_sec) * OSTICKS_PER_SEC);
    if (RTC_LMIC.globalDutyAvail < 0)
    {
        RTC_LMIC.globalDutyAvail = 0;
    }
#endif    
}

static void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : 
      Serial.println("Wakeup caused by external signal using RTC_IO"); 
      digitalWrite(LED_PIN_RED, HIGH);
      delay(500); 
      digitalWrite(LED_PIN_RED, LOW);
      if(sys.fall_detection == 0 )
      {
        button_Count++;
        Pedometer_Count++;
        sys.tdc = sys.sys_time;
        TDC_Time = sys.sys_time;
        TDC_Count = 0;
        Serial.printf("button_Count:%d\r\n",button_Count);
        if(sys.pedometer == 0) 
        {
          button_Count1 =1;
  //        sys.keep_flag =1;
          intwk_flag =1;
          sys.gps_start = 2;        
          myIMU.imu_power_down();       
          if(button_Count1 == 1)
          {
            turn_interrupts = 1;
            interrupts_flag = 0;        
          }        
        }
        else
        {
          sys.gps_start = 0;  
          button_Count1 =0;
          sys.exti_flag = 2;  
        }
      }
      else
      {
        sys.gps_start == 0;
        interrupts_flag = 1;
        sys.gps_alarm = 1;
        sys.alarm = 1;
        sys.exti_flag = 1;
        sys.buzzer_flag = 1;
        sys.alarm_no = 1;        
      }
    break;
    case ESP_SLEEP_WAKEUP_EXT1 : 
      gpio_deep_sleep_hold_dis();
      sys.gps_start = 0;
      sys.exti_flag = 0;
      EXIT_flag = 1;
      sport_mod =0;
      DFR =0;     
      if(sys.pedometer == 1)
        {
          myIMU.imu_power_down();
          Pedometer_OFF =1;
        }       

      Serial.println("Wakeup caused by external signal using RTC_CNTL"); 
    break;
    case ESP_SLEEP_WAKEUP_TIMER :     
    if(sys.Intwk == 1 && button_Count1 != 0 && sys.pedometer == 0 && TDC_flag == 1)
    {
      interrupts_flag = 1; 
    }
    if(sys.Intwk == 0) 
    {
      sport_mod =0;
      sys.alarm = 0;
      button_Count1 = 0;
      turn_interrupts = 0;      
    } 
    if(sys.pedometer == 1)
    { 
      sys.tdc = sys.sys_time;
      if(sys.tdc <= 4200000)
      {
        Pedometer_Count = 0;
      }
      else if(EXIT_flag != 1 && button_Count1 == 0 &&  sys.tdc >= 4200000)
      {
        if(TDC_Count !=0 )
        {
         Pedometer_Count = 0;
        }
        if(TDC_Time <= 3600000 )
        {
         Pedometer_Count = 0;
         Serial.println("TDC_Time <= 3600000"); 
        }  
      }
    }
    Serial.println("Wakeup caused by timer");
    break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default :
      Serial.printf("Wakeup was not caused by deep sleep: %d\r\n",wakeup_reason); 
      sys.config_Read();
      sys.gps_alarm = 0;
      sys.gps_start = 1;
      sys.exit_off = 1;
      sys.alarm = 0;
      sys.alarm_count =0;
      os_JOINED_flag = 1;
      TDC_Time = sys.sys_time;
      TDC_Count = 0;
      Serial.println(Pro_model "," Pro_version);
      Device_status();
      sys.Band_information();
      sys.loggpsdata_send = 0;
      if(sys.sensor_mode ==0 )
      {
        sys.sensor_mode = 1;
        sys.save_sensor_mode = sys.sensor_mode;
      } 
      if(sys.Intwk == 1)
      { 
        sys.keep_flag = 0;    
      }
      else
      {
        sys.keep_flag = 1;
      }
      if(sys.fire_version_write == 0)
      {
        sys.fire_version_write = sys.fire_version ;
        sys.FDR_flag = 1;
        sys.config_Write();  
      }  
      if(sys.fire_version_write != sys.fire_version )
      {
        sys.FDR_flag = 1;
        sys.fire_version_write = sys.fire_version ;
        sys.config_Write();    
      }
      else
      {
        sys.fire_version_write = sys.fire_version ;   
      } 
      if(sys.FDR_flag == 1)
      {
        sys.DATA_CLEAR();
        ESP.restart();                    
      }
      if(sys.FDR_flag == 0)
      {
        sys.atst = 15;   
        sys.FDR_flag = 2; 
        sys.showid = 0; 
        sys.lon = 1;   
        sys.pdop_value = 7.00;
        sys.Positioning_time = 180000;
        sys.TF[0] ={0x14};   
      }
      sys.tdc = sys.sys_time;
      gpio_deep_sleep_hold_dis();
      gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);       
      sys.config_Write();  
      digitalWrite(LED_PIN_BLUE, HIGH);
      delay(500); 
      digitalWrite(LED_PIN_BLUE, LOW); 
      digitalWrite(LED_PIN_RED, HIGH);
      delay(500); 
      digitalWrite(LED_PIN_RED, LOW);   
      digitalWrite(LED_PIN_GREEN, HIGH);
      delay(500); 
      digitalWrite(LED_PIN_GREEN, LOW);          
    break;
  }
}

void setup() {
// put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200); 
  sys.eeprom_init();
  sys.config_Read();
  print_wakeup_reason();
  button_event_init();
// LMIC init
  os_init();
// Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  if(sys.channel_single < 8)
  {
    #if defined( CFG_us915 ) || defined( CFG_au915 )
    LMIC_selectSubBand(sys.channel_single);
    #endif
  }
  #if defined( CFG_as923 ) || defined( CFG_au915 ) 
  Serial.printf("sys.Dwelltime:%d\r\n",sys.Dwelltime); 
  LMIC_setdwellTime(sys.Dwelltime);
  #endif 
  if (RTC_LMIC.seqnoUp != 0)
  {
    LMIC = RTC_LMIC;
  }
  Serial.printf("TDC:");   
  if(turn_interrupts == 1)
  {
    Serial.println(sys.mtdc);      
  } 
  else
  {
    if(sport_mod == 0)
    {
      if(sys.loggpsdata_send == 0)
      {
        sys.tdc = sys.sys_time;
        Serial.println(sys.tdc); 
      }
      else
      {
       Serial.println(10000); 
      }
    }
    else
    {
      Serial.println(sys.atdc); 
    }
  } 
  sys.sensor_mode = sys.save_sensor_mode;  
  sensor.bat = BatGet(); 
  Serial.printf("BAT:%d mV\r\n",sensor.bat); 
  if(EXIT_flag != 1 && button_Count1 == 0 &&  sys.tdc >= 4200000)
  {
    if(TDC_Count !=0 )
    {
      Serial.printf("TDC_Count:%d\r\n",TDC_Count); 
      sys.gps_start = 3;
      sys.exti_flag = 2;
      sport_Count = 0;
    }
    if(TDC_Time <= 3600000 )
    {
      sys.gps_start = 2;
      sys.exti_flag = 4;
      interrupts_flag= 0;
    }  
  }
  if(sport_mod== 0)
  {
    if(sys.Intwk == 1)
    {      
       LIS3DH_configIntterupts();       
    }     
  }
  if(sys.fall_detection == 1 )
  {
    LIS3DH_configIntterupts(); 
  }
  if(sys.pedometer == 1&&Pedometer_OFF ==0)
  {
     if(sys.gps_alarm == 0)
     {
       LIS3DH_configIntterupts();
     }     
  }   
  if(sensor.bat<2840)
  {
    sys.gps_work_flag = false;
    sensor.latitude  = 0xFFFFFFFF;
    sensor.longitude = 0xFFFFFFFF;
    Serial.println("Low voltage,do not start GPS");
  }
  else if(sys.gps_start == 2 && sys.loggpsdata_send == 0 && interrupts_flag == 0)
  {
    if(sys.sensor_mode == 1)
    {         
      GPS_Init();
      GPS_boot();
      Serial.println("Start searching for GPS...");     
      if(sys.Positioning_time != 0)
      {
//定时器
        timerSemaphore = xSemaphoreCreateBinary();
        timer = timerBegin(0, 80, true);
        timerAttachInterrupt(timer, &onTimer, true);
        timerAlarmWrite(timer, sys.Positioning_time*1000, true);
        timerAlarmEnable(timer);
      }         
    }
    else if(sys.sensor_mode == 2 || sys.sensor_mode == 3)
    {
      if(sys.sensor_mode == 2)
      {      
        if(sys.ble_mod == 1) 
        {  
          Serial.println("Start searching for BLE..."); 
        }
        else if(sys.ble_mod == 2) 
        {  
          Serial.println("Start searching for WIFI..."); 
        }        
      }
      else if(sys.sensor_mode == 3)
      {
       Serial.println("Enable ble and gps hybrid mode"); 
      }
      if(sys.ble_mod == 1) 
      {       
        ble_init();
        ble_run();
      }
      else if(sys.ble_mod == 2) 
      {
        wifi_scan();
      }
      sys.mod = sys.save_mode;
    }      
    sys.alarm_count =0;
    os_JOINED_flag = 0;     
  } 
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)//串口接收到数据
  { 
    memset(rxData,0,maxRxSize);
    Serial.readBytes(rxData,Serial.available());
    ATEerror_t AT_State = ATInsPro(rxData);
    if(AT_State == AT_OK)
      Serial.println(ATError_description[AT_OK]);
    else if(AT_State == AT_PARAM_ERROR)
      Serial.println(ATError_description[AT_PARAM_ERROR]);
    else if(AT_State == AT_BUSY_ERROR)
      Serial.println(ATError_description[AT_BUSY_ERROR]);
    else if(AT_State == AT_TEST_PARAM_OVERFLOW)
      Serial.println(ATError_description[AT_TEST_PARAM_OVERFLOW]);
    else if(AT_State == AT_RX_ERROR)
      Serial.println(ATError_description[AT_RX_ERROR]);
    else 
      Serial.println(ATError_description[AT_ERROR]);
  }

  button_attach_loop();
  if(sys.Intwk == 1 && sport_mod == 0 && sys.pedometer == 0)
  {
    if(interrupts_flag == 1)
    {  
      if (millis() - sleep_last > 2000)
      {
        LIS3DH_configIntterupts();
        interrupts_flag = 2; 
        sleep_last = millis();
        if(interrupts_count == 1)
        {
           sys.tdc = sys.sys_time;
           sys.keep_flag = 1;
           sys.exti_flag = 3;
           turn_interrupts = 1;
           Serial.printf("keep_flag:%d\r\n",sys.keep_flag);  
           myIMU.imu_power_down();  
           interrupts_count = 0;
        } 
        else
        {
           sys.keep_flag = 0;
           sys.exti_flag = 2;
           uint32_t ss =TDC_Time-sys.mtdc;
           if(sport_Count == 0)
           {
             if(TDC_Time <= sys.mtdc)
             {
                sys.keep_flag = 0;
                sys.exti_flag = 3;  
                button_Count1 = 0;           
             }
             else
             {
               if(ss <20000)
               {
                 sys.tdc = ss;
                 LIS3DH_configIntterupts();
               }
               else
               {
                  TDC_Time = sys.sys_time;
//                  tdc_time();
                  if(sys.tdc >=4200000)
                  {
                    sys.tdc = 3600000-sys.mtdc-10000;
                    sys.gps_start = 3;
                    sys.exti_flag = 2;                    
                  }
                  else
                  {
                    sys.tdc = sys.tdc-sys.mtdc-10000;
                  }
                  TDC_Count ++;
                  LIS3DH_configIntterupts();
               }
               sport_Count = 1;
               button_Count1 = 0;
             }
             interrupts_count = 0;
             turn_interrupts = 0;
           }         
        }    
      }                 
    }
  }
  
  alarm_state();
  
  if (sys.gps_start == 1 )
  {
    device_start();
  }   
  
  gps_send();  
  
  if(sys.device_flag == 0)   
  {
    sys_sleep();
  }
}

void gps_send(void)
{
  if(sys.gps_work_flag == true )
  {
    if(GPS_DATA() == true )
    {
      os_JOINED_flag = 0;
      sys.gps_work_flag = false;
      timerAlarmDisable(timer);
    }
    else if(sys.loggpsdata_send == 1 || sys.ble_flag ==1)
    {
      os_JOINED_flag = 0;
      sys.gps_work_flag = false;     
    }
  }     
  if(sys.gps_work_flag==false && sys.collect_sensor_flag == false )
  {
    sys.collect_sensor_flag = true;
    os_run_flag = true;
    do_send(&sendjob);
  }
  if(os_run_flag == true)
      os_runloop_once();  
}

void tdc_time(void)
{
  uint8_t TDC_Flag = sys.sys_time/3600000;
  if(TDC_Flag>0)
  {  
    if(TDC_Time <= 3600000)
    {
      sys.exti_flag = 3;
      TDC_Count = 0; 
      TDC_Time = sys.sys_time;
      if(TDC_Count == 0)
      {
        sys.tdc = 3600000;
      }      
      Serial.printf("TDC_Time:%d\r\n",TDC_Time);            
    }
    else
    {
      if(TDC_Count == 0)
      {
        sys.tdc = 3600000;
      }
      else
      {
        sys.tdc = 3600000-3000;
        TDC_Time = TDC_Time -3600000; 
        if(TDC_Time <= 3600000 )
        {
          sys.gps_start = 2 ;
          TDC_Count = 0; 
          sys.tdc =TDC_Time;
          Serial.printf("TDC_Time:60000\r\n"); 
        }
      }
      Serial.printf("TDC:%d\r\n",sys.tdc );   
      Serial.printf("TDC_Time:%d\r\n",TDC_Time); 
    }
  }
}

void device_start(void)
{
  sys.gps_work_flag = false;
  if(sys.gps_work_flag==false && sys.collect_sensor_flag == false )
  {
    sys.collect_sensor_flag = true;
    os_run_flag = true;
    device_send(&sendjob);
  }
  if(os_run_flag == true)
      os_runloop_once(); 
      sys.device_flag = 0;
      sys.gps_start = 2;
}

void Alarm_start(void)
{
  sys.gps_work_flag = false;
  turn_interrupts = 0;
  if(sys.gps_work_flag==false && sys.collect_sensor_flag == false )
  {
    sys.collect_sensor_flag = true;
    os_run_flag = true;
    Alarm_send(&sendjob);
  }
  if(os_run_flag == true)
      os_runloop_once(); 
}

void sys_sleep(void)
{
  const bool timeCriticalJobs = os_queryTimeCriticalJobs(ms2osticksRound((sys.tdc)));
  if (!timeCriticalJobs && send_complete == true && !(LMIC.opmode & OP_TXRXPEND))
  {
    GXHT3x_LowPower();
    
    if(sys.keep_flag == 1)
    {
//      Serial.println("imu_power_down");
      LIS3DH_configIntterupts();
      myIMU.imu_power_down();  
    }
    digitalWrite(BAT_PIN_LOW, LOW); 
    sys.config_Write();              
    if(sys.gps_start == 0)
    {   
      if(sys.alarm_count<=60)
      {     
        sys.gps_start = 0;
        os_JOINED_flag = 0;
        if(sys.alarm_count == 60)
        {
          sys.alarm = 0;
          sys.gps_alarm = 0;
          if(sys.Intwk == 1)
          {
             LIS3DH_configIntterupts();         
          }                
        }
        else
        {
          sys.alarm = 1;
          sys.gps_alarm = 1;
          LIS3DH_configIntterupts(); 
          myIMU.imu_power_down(); 
          SaveLMICToRTC(sys.atdc);  
        }
        sys.config_Write();        
      }
    }
    if(os_JOINED_flag == 1)
    {
      sys.gps_start = 2; 
      sys.config_Write();
      sys.tdc = 1000;             
    }
    else
    {
      if(EXIT_flag != 1 && button_Count1 == 0 && sys.tdc >= 4200000)
      {
       tdc_time();
       TDC_Count ++;   
      }
    }
//    os_JOINED_flag = 1;
    Serial.println("Enter sleep mode");
    if(turn_interrupts == 1)
    {
      SaveLMICToRTC(sys.mtdc); 
      LIS3DH_configIntterupts();
      myIMU.imu_power_down();
    }
    else 
    {
      if(sys.alarm == 0)
      {
        if(sys.loggpsdata_send == 0)
        {
          if(os_JOINED_flag == 0)
          {
            sys.tdc = sys.sys_time;
          }
           SaveLMICToRTC(sys.tdc);
        }
        else
        {
         SaveLMICToRTC(10000); 
        }
      }
      else
      {
        SaveLMICToRTC(sys.atdc);
      }  
    }  
    GPS_shutdown();
    LMIC_shutdown();  
    sys.LORA_EnterSleepMode();
    rtc_gpio_isolate(GPIO_NUM_27);//mosi
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,1); //1 = High, 0 = Low
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ALL_LOW ); //1 = High, 0 = Low       
    gpio_reset();
    if(turn_interrupts == 1)
    {
      LIS3DH_configIntterupts();
      myIMU.imu_power_down();
      TDC_flag = 1;
      esp_sleep_enable_timer_wakeup(sys.mtdc*1000); //设置定时器唤醒 
      Serial.printf("KEEP TDC:%d\r\n",sys.mtdc);      
    } 
    else
    {
      if(sys.alarm == 0)
      {
        if(sys.loggpsdata_send == 0)
        {
         if(os_JOINED_flag == 0)
         {   
           TDC_Count ++;
         }
         os_JOINED_flag = 0;
         sport_mod = 0;
         TDC_flag = 0;
         esp_sleep_enable_timer_wakeup(sys.tdc*1000); //设置定时器唤醒 
         Serial.printf("TDC:%d\r\n",sys.tdc);
        }
        else
        {
         esp_sleep_enable_timer_wakeup(10000*1000); //设置定时器唤醒 
        }
      }
      else
      {
        LIS3DH_configIntterupts();
        myIMU.imu_power_down();     
        esp_sleep_enable_timer_wakeup(sys.atdc*1000); //设置定时器唤醒
      }
    }
    Serial.end(); 
    esp_deep_sleep_start(); //启动DeepSleep
  }
}
void alarm_state(void)
{
  if(sys.ble_flag == 2)
  {
    sys.exti_flag = 3;
    sys.sensor_mode = 1;    
    sys.ble_flag = 0; 
  }
  if(sys.exti_flag == 1)
  {
    if(sys.gps_alarm == 1)
    {
      if(sys.buzzer_flag == 1 && sys.alarm_no ==1)
      {
        if(sys.lon == 1)
        { 
          buzzer();  
          digitalWrite(LED_PIN_RED, LOW);     
          delay(5000);
//          Serial.println("stop");
          Stop_buzzer();
          sys.keep_flag = 1;
        } 
        LIS3DH_configIntterupts();
        myIMU.imu_power_down();       
        sys.alarm_no = 0;
        sport_mod =1;
        sys.keep_flag = 1;
        Alarm_start();
      }
      else if(sys.buzzer_flag == 0)
      {
        sys.alarm_count++; 
        Serial.printf("send NO.%d Alarm data \n\r",sys.alarm_count);    
        sys.gps_alarm =0 ;
        sys.alarm = 1;
        sport_mod =1;
        sys.buzzer_flag = 2;
        sys.exti_flag = 3;
        if(sys.lon == 1)
        {
          digitalWrite(LED_PIN_RED, HIGH);
          delay(1000);   
          digitalWrite(LED_PIN_RED, LOW);  
        }          
      }
    }
    if(sys.alarm_count == 60)
    {
      sys.keep_flag = 0;    
      sys.gps_alarm = 0;
      sys.gps_start = 2;
      sys.loggpsdata_send = 0;
      interrupts_flag = 0;
      sys.alarm = 0;
      sys.alarm_count =0;
      sys.exti_flag = 3;
      sys.config_Write(); 
      if(sys.Intwk == 1)
      {
         LIS3DH_configIntterupts();         
      }       
      Serial.println("Exit Alarm");  
      if(sys.lon == 1)
      {
        digitalWrite(LED_PIN_RED, HIGH);
        delay(1000);   
        digitalWrite(LED_PIN_RED, LOW);  
      }   
    }
  }
  else if (sys.exti_flag == 0)
  {
    if(sys.sleep_flag == 0)
    {
      uint32_t atsend = sys.atst*1000;
      if (millis() - sleep_last > atsend)
      {
        sys.exti_flag = 2;
        sleep_last = millis();
      } 
    }     
  }
  else if (sys.exti_flag == 2)
  { 
    if(sys.alarm == 1)
    {
      sys.exti_flag = 1;
    }
    else 
    {
      sys.exti_flag = 3;
      sys.gps_start = 2;
    }
    sys.config_Write();       
    Serial.println("sleep mode");
    GXHT3x_LowPower();
    if(sys.Intwk == 0 || button_Count1 == 1)
    {   
      if(sys.pedometer == 0)
      {
        LIS3DH_configIntterupts();
        myIMU.imu_power_down();  
      }
    }
    if(sys.Intwk == 1 && EXIT_flag == 1)
    {
      turn_interrupts = 1; 
    }   
    digitalWrite(BAT_PIN_LOW, LOW); 
    sys.config_Write();              
    if(sys.gps_start == 0)
    {   
      if(sys.alarm_count<=60)
      {          
        sys.gps_start = 0;
        os_JOINED_flag = 0;
        if(sys.alarm_count == 60)
        {
          sys.alarm = 0;
          sys.gps_alarm = 0;
          if(sys.Intwk == 1)
          {
             LIS3DH_configIntterupts();         
          }           
        }
        else
        {
          sys.alarm = 1;
          sys.gps_alarm = 1; 
          LIS3DH_configIntterupts();
          myIMU.imu_power_down();  
          SaveLMICToRTC(sys.atdc);  
        }
        sys.config_Write();        
      }
    }
    if(os_JOINED_flag == 1)
    {
      sys.gps_start = 2; 
      sys.config_Write();
      os_JOINED_flag = 0;
      sys.tdc = 1000;             
    }
    else
    {
      if(EXIT_flag != 1 && button_Count1 == 0 && sport_Count == 0)
      {
       tdc_time();
       TDC_Count ++;
       if(sys.pedometer == 1)
       {
         sys.tdc = sys.mtdc-Pedometer_Count*2000;   
//         Serial.printf("pedometer:%d\r",Pedometer_Count); 
       }
      }
      if(EXIT_flag == 1 && button_Count1 == 0 )
      {
        if(sys.tdc>=3600000)
        {
          sys.tdc =3600000-sys.atst*1000;
        }
        else if(sys.tdc >= (sys.tdc-sys.atst*1000))
        {
         sys.tdc =sys.tdc-sys.atst*1000;      
        }
        EXIT_flag = 0;
      }
    }    
    if(turn_interrupts == 1)
    {
      EXIT_flag = 0;
      SaveLMICToRTC(sys.mtdc); 
    }
    else 
    {
      if(sys.alarm == 0)
      {
        if(sys.loggpsdata_send == 0)
        {
         SaveLMICToRTC(sys.tdc);
        }
        else
        {
         SaveLMICToRTC(10000); 
        }
      }
      else
      {
        SaveLMICToRTC(sys.atdc);
      }  
    }  
    GPS_shutdown();
    LMIC_shutdown();  
    sys.LORA_EnterSleepMode();
    rtc_gpio_isolate(GPIO_NUM_27);//mosi
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,1); //1 = High, 0 = Low
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ALL_LOW ); //1 = High, 0 = Low       
    gpio_reset();
    if(turn_interrupts == 1)
    {
      esp_sleep_enable_timer_wakeup(sys.mtdc*1000); //设置定时器唤醒 
      TDC_flag = 1;
      Serial.printf("KEEP TDC:%d\r\n",sys.mtdc);      
    } 
    else
    {
      if(sys.alarm == 0)
      {
        if(sys.loggpsdata_send == 0)
        {
           esp_sleep_enable_timer_wakeup(sys.tdc*1000); //设置定时器唤醒 
           TDC_flag = 0;
          if(sys.pedometer == 1)
          {
           
           Serial.printf("KEEP TDC:%d\r\n",sys.mtdc);  
          }
          else
          {
            Serial.printf("TDC:%d\r\n",sys.tdc);
          }
        }
        else
        {
          esp_sleep_enable_timer_wakeup(10000*1000); //设置定时器唤醒
        }
      }
      else
      {
        esp_sleep_enable_timer_wakeup(sys.atdc*1000); //设置定时器唤醒
      }
    }
    Serial.end();
    esp_deep_sleep_start(); //启动DeepSleep
  }
  else if (sys.exti_flag == 3)
  {   
     if(sys.sensor_mode == 1)
      {
        GPS_Init();
        GPS_boot();
        Serial.println("Start searching for GPS...");
        if(sys.Positioning_time != 0)
        {
  //定时器
          timerSemaphore = xSemaphoreCreateBinary();
          timer = timerBegin(0, 80, true);
          timerAttachInterrupt(timer, &onTimer, true);
          timerAlarmWrite(timer, sys.Positioning_time*1000, true);
          timerAlarmEnable(timer);
        }
        sys.exti_flag = 4; 
      }
      else if(sys.sensor_mode == 2 || sys.sensor_mode == 3)
      {
        if(sys.sensor_mode == 2)
        {      
          if(sys.ble_mod == 1) 
          {  
            Serial.println("Start searching for BLE..."); 
          }
          else if(sys.ble_mod == 2) 
          {  
            Serial.println("Start searching for WIFI..."); 
          }        
        }
        else if(sys.sensor_mode == 3)
        {
         Serial.println("Enable ble and gps hybrid mode"); 
        }
        if(sys.ble_mod == 1) 
        {       
          ble_init();
          ble_run();
        }
        else if(sys.ble_mod == 2) 
        {
          wifi_scan();
        }                   
        if(button_Count1  == 1)
        {
          myIMU.imu_power_down();  
        }
        sys.mod = sys.save_mode;
        sys.exti_flag = 4; 
      }     
  }  
}

void LIS3DH_configIntterupts(void)
{
  if(sys.fall_detection == 0 )
  {
    myIMU.begin(sampleRate, 1, 1, 1, accelRange);
  //  uint8_t dataToWrite = 0;
    myIMU.writeRegister(LIS3DH_CTRL_REG1, 0x3f);
    myIMU.writeRegister(LIS3DH_CTRL_REG2, 0x00);
    myIMU.writeRegister(LIS3DH_CTRL_REG3, 0x40);
    myIMU.writeRegister(LIS3DH_CTRL_REG4, 0x00);
    myIMU.writeRegister(LIS3DH_CTRL_REG5, 0x00);
    myIMU.writeRegister(LIS3DH_INT1_THS, sys.TF[0]);
    myIMU.writeRegister(LIS3DH_INT1_DURATION, 0x82);
    myIMU.writeRegister(LIS3DH_INT1_CFG, 0x0A);
    myIMU.writeRegister(LIS3DH_CTRL_REG6, 0x00);   
    uint8_t dataRead;
    myIMU.readRegister(&dataRead, LIS3DH_INT1_SRC);//cleared by reading     
    if(dataRead & 0x40)
    {
      interrupts_count = 1;
      sport_Count =0 ;
      if(sys.showid == 1)
      {    
        Serial.println("Interrupt Active");
      }
    }
    if(sport_Count == 1 )
    {
       interrupts_flag = 2; 
       sys.keep_flag = 0;
       sys.exti_flag = 3;
       interrupts_count = 0;
       sport_mod = 0;
       sport_Count = 0;
       sys.tdc = sys.sys_time;
    } 
    if(Pedometer_Count==0 && DFR == 1 )
    {
     sys.gps_start = 2;
     sys.loggpsdata_send = 0;
     interrupts_flag = 0; 
    }    
  }
  else
  {
    myIMU.begin(sampleRate, 1, 1, 1, accelRange);
  //  uint8_t dataToWrite = 0;
    myIMU.writeRegister(LIS3DH_CTRL_REG1, 0x3f);
    myIMU.writeRegister(LIS3DH_CTRL_REG2, 0x09);
    myIMU.writeRegister(LIS3DH_CTRL_REG3, 0x40);
    myIMU.writeRegister(LIS3DH_CTRL_REG4, 0x00);
    myIMU.writeRegister(LIS3DH_CTRL_REG5, 0x00);
    myIMU.writeRegister(LIS3DH_INT1_THS, sys.TF[0]);
    myIMU.writeRegister(LIS3DH_INT1_DURATION, 0x82);
    myIMU.writeRegister(LIS3DH_INT1_CFG, 0x2A);
    myIMU.writeRegister(LIS3DH_CTRL_REG6, 0x00);    
    uint8_t dataRead;
    myIMU.readRegister(&dataRead, LIS3DH_INT1_SRC);//cleared by reading
    
  }
//  if(sys.showid == 1)
//  {
//    Serial.print("LIS3DH_INT1_SRC: 0x");
//    Serial.println(dataRead, HEX);
//    Serial.println("Decoded events:");
//  }
  delay(100); 
}

static void LORA_RxData(uint8_t *AppData, uint8_t AppData_Len)
{
  bool store_flag = false;
  if(sys.lon == 1)
  {
    digitalWrite(LED_PIN_GREEN, HIGH);
    delay(1000); 
    digitalWrite(LED_PIN_GREEN, LOW); 
  }    
  switch (AppData[0] & 0xff)
  {
    case 0x01:
    {
      if (AppData_Len == 4) //---->AT+TDC
      {
        uint32_t ServerSetTDC = (AppData[1] << 16 | AppData[2] << 8 | AppData[3]); // S
        if (ServerSetTDC < 6)
        {
          Serial.println("Invalid parameter");
        }
        else
        {
          store_flag = true;
          sys.tdc = ServerSetTDC * 1000;
          sys.sys_time = ServerSetTDC * 1000;         
        }
      }
    }
    break;   
    case 0x02:
    {
      store_flag = true;
      if(( AppData_Len == 2 )&&(AppData[1]==0x01)) //Exit Alarm
      {
          sys.gps_alarm = 0;
          sys.gps_start = 2;
          sys.alarm = 0;
          sys.alarm_count =0;
          sys.gps_work_flag = false;
          sys.config_Write(); 
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_BLUE, HIGH);
            if (millis() - Exit_Alarm > 5000)
            {
              digitalWrite(LED_PIN_BLUE, LOW);  
              Exit_Alarm = millis();
            }   
          } 
          if(sys.Intwk == 1)
          {
             LIS3DH_configIntterupts();
             sys.keep_flag = 0;         
          }           
          Serial.println("Exit Alarm");          
      }
    }
    break;  
    case 0x03:
    {
      if (AppData_Len == 4) //---->AT+MTDC
      {
        uint32_t ServerSetTDC = (AppData[1] << 16 | AppData[2] << 8 | AppData[3]); // S
        if (ServerSetTDC < 6)
        {
          Serial.println("Invalid parameter");
        }
        else
        {
          store_flag = true;
          sys.mtdc = ServerSetTDC * 1000;
        }
      }
    }
    break;       
    case 0x04:
    {
      if (AppData_Len == 2 && AppData[1] == 0xFF) //---->ATZ
      {
        ESP.restart();
      }
    }
    break;    
    case 0x05:
    {
      store_flag = true;
      if (AppData_Len == 2 && AppData[1] == 0x01) //---->AT+CFM=1
      {
        sys.frame_flag =1;
      }
      else if(AppData_Len == 2 && AppData[1] == 0x00) //---->AT+CFM=0
      {
        sys.frame_flag =0;
      }      
    }    
    break;
    case 0x22:
    {
      store_flag = true;
      if(( AppData_Len == 2 )&&(AppData[1]==0x01))   //---->AT+ADR=1
      {   
        LMIC_setAdrMode(0);
        bool _adr = 0;
        sys.LORA_SetADR(_adr);
      }
      else if(( AppData_Len == 4 )&&(AppData[1]==0x00)) //---->AT+ADR=0
      {
        LMIC_setAdrMode(1);
        bool _adr = 1;
        sys.LORA_SetADR(_adr);
        LMIC_setDrTxpow((AppData[2]<8)?AppData[2]:sys.LORA_GetDR(),
                        (AppData[3]!=0xFF)?AppData[3]:sys.LORA_GetTXP());
      }
    }
    break;
    case 0x23:
    {
      store_flag = true;
      if(( AppData_Len == 2 )&&(AppData[1]==0x01)) 
      {
        sys.gps_start = 1;
        Serial.println("device");
        sys.device_flag = 1;
        sys.collect_sensor_flag = false;
      }
    }    
    break;
    case 0x24:
    {
      #if defined( CFG_us915 ) ||defined( CFG_au915 ) 
      store_flag = true;
      if( AppData_Len == 2 )
      {
        sys.channel_single = AppData[1];
        LMIC_selectSubBand(sys.channel_single);
      }
      #endif 
    }    
    break;   
    case 0x25:
    {
      #if defined( CFG_as923 ) ||defined( CFG_au915 ) 
      store_flag = true;
      if( AppData_Len == 2 )
      {
        sys.Dwelltime = AppData[1];
      }
      #endif 
    }    
    break;   
    case 0x34:
    {
      store_flag = true;
      if( AppData_Len == 2 )
      {
        sys.PNACKmd = AppData[1];
      }
    }    
    break;        
    case 0xa5:
    {
       store_flag = true;
       if(AppData_Len == 2 )//AT+MD = 1
       {
        sys.mod = AppData[1];
        sys.save_mode = AppData[1];
       }
       if(AppData_Len == 3 )//AT+SMOD = 1,0
       {
        sys.sensor_mode = AppData[1];
        sys.save_sensor_mode = AppData[1];
        sys.mod = AppData[2];
        sys.save_mode = AppData[2];
       }
    }
    break;
    case 0xaa:
    {
      store_flag = true;
      if( AppData_Len == 3 )
      {
        sys.Positioning_time = (AppData[1]<<8 |AppData[2]);
        sys.Positioning_time = sys.Positioning_time*1000;
      }
    }
    break;
    case 0xab:
    {
      store_flag = true;
      if( AppData_Len == 2 )
      {
        sys.fr_mode = AppData[1];
      }
    }
    break;  
    case 0xac:
    {
      store_flag = true;
      if( AppData_Len == 2 )
      {
        sys.se_mode = AppData[1];
      }
    }
    break;  
    case 0xad:
    {
      store_flag = true;
      if( AppData_Len == 3 )
      {
        sys.pdop_value = (AppData[1]<<8 | AppData[2])/10.0;
      }
    }
    break;   
    case 0xae:
    {
      store_flag = true;
      if( AppData_Len == 2 )
      {
        sys.lon = AppData[1];
      }
    }
    break;  
    case 0xaf:
    {
      store_flag = true;
      if( AppData_Len == 2 )
      {
        sys.Intwk = AppData[1];
      }
    }
    break;  
    case 0xb1:
    {
      if (AppData_Len == 4) //---->AT+ATDC
      {
        uint32_t ServerSetTDC = (AppData[1] << 16 | AppData[2] << 8 | AppData[3]); // S
        if (ServerSetTDC < 6)
        {
          Serial.println("Invalid parameter");
        }
        else
        {
          store_flag = true;
          sys.atdc = ServerSetTDC * 1000;
        }
      }
    }
    break;  
    case 0xb2:
    {
      store_flag = true;
      if (AppData_Len <= 11) //---->AT+BLEMASK
      {
        for(int a=1,b=0;a < AppData_Len;a++)
        {
          sys.blemask_data[b++] =AppData[a]+'0';
        }
      }
    }
    break;   
    case 0xb3:
    {
      store_flag = true;
      if (AppData_Len <= 11) //---->AT+BLEMASK
      {
        for(int a=1,b=0;a < AppData_Len;a++)
        {
          sys.wifimask_data[b++] =AppData[a]+'0';
        }
      }
    }
    break;       
    case 0xb4:
    {
      store_flag = true;
      if (AppData_Len == 2) //---->AT+PT
      {
       sys.TF[0] =AppData[1];
      }
    }
    break;  
    case 0xb5:
    {
      store_flag = true;
      if (AppData_Len == 2) //---->AT+PM
      {
       sys.atst =AppData[1];
      }
    }
    break;     
    case 0xb6:
    {
      store_flag = true;
      if (AppData_Len == 2) //---->AT+PM
      {
       sys.pedometer =AppData[1];
      }
    }
    break; 
    case 0xb7:
    {
      store_flag = true;
      if (AppData_Len == 2) //---->AT+FD
      {
       sys.fall_detection =AppData[1];
      }
    }
    break;                         
 //...
    default:
      Serial.println("Unknown instruction");
      break;
  }

  if (store_flag == true)
  {
    sys.config_Write();
  }
}

int hexToint(char *str)
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
