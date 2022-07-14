#include <SPI.h>
#include "common.h"
#include "lis3dh-motion-detection.h"

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
unsigned long buzzerlast = 0UL;

unsigned long sleep_last = 0UL;
static char rxData[maxRxSize]={0};
hw_timer_t * timer = NULL;
hw_timer_t * timer1 = NULL;
volatile SemaphoreHandle_t timerSemaphore;
volatile SemaphoreHandle_t timerSemaphore1;
// Saves the LMIC structure during DeepSleep
RTC_DATA_ATTR lmic_t RTC_LMIC;
RTC_DATA_ATTR int button_Count = 0;

void os_getArtEui (u1_t* buf) { sys.LORA_GetAEUI(buf);}

void os_getDevEui (u1_t* buf) { sys.LORA_GetDEUI(buf);}

void os_getDevKey (u1_t* buf) { sys.LORA_GetAKEY(buf);}

static void LORA_RxData( uint8_t *AppData,uint8_t AppData_Len);
void gps_send(void);
void device_start(void);
void sys_sleep(void);
void alarm_state(void);

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
          Serial.println(F("EV_JOINING"));
          break;
      case EV_JOINED:
          Serial.println(F("EV_JOINED"));
          os_JOINED_flag = 1;
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_GREEN, HIGH);
            delay(1000);
          }                            
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
          if (LMIC.txrxFlags & TXRX_DNW1)
          {
            Serial.print("RX1 on freq: "); Serial.print(LMIC.freq  / 1000000);
            Serial.print("."); Serial.print((LMIC.freq / 100000) % 10);
            Serial.print("MHz");
            Serial.print("  LMIC.datarate: "); Serial.print(LMIC.datarate);
            Serial.print("  LMIC.txpow: "); Serial.println(LMIC.txpow);   
          } 
          if (LMIC.txrxFlags & TXRX_DNW2)   
          {
            Serial.print("RX2 on freq: "); Serial.print(LMIC.freq  / 1000000);
            Serial.print("."); Serial.print((LMIC.freq / 100000) % 10);
            Serial.print("MHz");
            Serial.print("  LMIC.datarate: "); Serial.print(LMIC.datarate);
            Serial.print("  LMIC.txpow: "); Serial.println(LMIC.txpow);   
          }          
      
          Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
          if (LMIC.txrxFlags & TXRX_ACK)
          {
            sys.frame_ACK = 0;
            if(sys.addr_gps_write == sys.addr_gps_read)
            {
              sys.GPSDATA_CLEAR();
              sys.addr_gps_read = 0;
              sys.addr_gps_write = 0;
              sys.loggpsdata_send = 0;
            }
            if(sys.addr_gps_write >=14)
            {
              sys.loggpsdata_send = 1;
            }
            if((LMIC.opmode & OP_NEXTCHNL)||(LMIC.opmode & OP_RNDTX))
            {
                #if !defined(CFG_eu868)||!defined(CFG_us915)||!defined(CFG_kr920)|| !defined(CFG_in866)||!defined(CFG_au915)
    //            LMIC.opmode = OP_RNDTX ; 
                LMIC.opmode = OP_NEXTCHNL ;
//                LoraWANPrintLMICOpmode();
//                Serial.println();  
    //             sys_sleep();            
                #endif      
            } 
            Serial.println(F("Received ack"));           
            send_complete = true;
            break;
          }            
          else if (LMIC.txrxFlags & TXRX_NACK)
          {
            sys.config_Read();
            if(sys.addr_gps_write>0&&sys.addr_gps_write<14)
            {
              sys.GPSDATA_CLEAR();
              sys.addr_gps_write =0 ;
            }
            sys.gps_data_Weite();
            sys.frame_ACK = 1;
            sys.loggpsdata_send = 0;  
            Serial.println(F("Received nack"));         
          }
          if((LMIC.opmode & OP_NEXTCHNL)||(LMIC.opmode & OP_RNDTX))
          {
              #if defined( CFG_as923 )
  //            LMIC.opmode = OP_RNDTX ; 
              LMIC.opmode = OP_NEXTCHNL ;
//              LoraWANPrintLMICOpmode();
//              Serial.println();  
  //             sys_sleep();            
              #endif      
          }           
          if (LMIC.dataLen)
          {
            Serial.print(F("Received:"));
            for(uint8_t i=0;i<LMIC.dataLen;i++)
              Serial.printf("%.2X ",LMIC.frame[i+LMIC.dataBeg]);
            Serial.println();
            LORA_RxData(&LMIC.frame[LMIC.dataBeg],LMIC.dataLen);            
          }
          if(sys.addr_gps_read <= 0)
          {
           sys.addr_gps_read = 0;
           Serial.printf("addr_gps_read =%d\n\r",sys.addr_gps_read);     
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
            delay(100);
          }
          if(os_JOINED_flag == 0)
          { 
            if(fcnt_flag == 1)
            {
              Serial.println("UpLinkCounter = 0");
            }
            else
            { 
              Serial.print("UpLinkCounter = ");
              Serial.println( LMIC.seqnoUp-1 );
            }
            Serial.print("TX on freq: "); Serial.print(LMIC.freq / 1000000);
            Serial.print("."); Serial.print((LMIC.freq / 100000) % 10);
            Serial.print("MHz");
            Serial.print("  LMIC.datarate: "); Serial.print(LMIC.datarate);
            Serial.print("  LMIC.txpow: "); Serial.println(LMIC.txpow);
          }
          if( (LMIC.txrxFlags & TXRX_DNW1) == 0 || (LMIC.txrxFlags & TXRX_NACK) !=0 || LMIC.dataLen == 0 )          
          {
            LMIC.txCnt = 8;
            Serial.println(F("Received nack"));      
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
      if(sys.loggpsdata_send == 1)
      {
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
        if(sys.sensor_mode == 1)
        {
          mydata[i++] = (sensor.latitude>>24)  & 0xFF;
          mydata[i++] = (sensor.latitude>>16)  & 0xFF;
          mydata[i++] = (sensor.latitude>>8)   & 0xFF;
          mydata[i++] = (sensor.latitude)      & 0xFF;
          mydata[i++] = (sensor.longitude>>24) & 0xFF;
          mydata[i++] = (sensor.longitude>>16) & 0xFF;
          mydata[i++] = (sensor.longitude>>8)  & 0xFF;
          mydata[i++] = (sensor.longitude)     & 0xFF;        
        }
      }
      if(sys.mod == 0)
      {
        sys.port = 2;
        mydata[i++] = ((sys.alarm<<6) | (sensor.bat>>8)) & 0xFF;
        mydata[i++] = (sensor.bat) & 0xFF;
        mydata[i++] = ((sys.mod<<6) | (sys.lon<<5) | sys.frame) & 0xFF;   
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
        mydata[i++] = ((sys.mod<<6) | (sys.lon<<5) | sys.frame) & 0xFF;           
      }
      else if(sys.mod == 2)
      {
        
      }           
//      if(sys.mod!=DEFAULT_MODE && sys.mod<GXHT3x_MODE)
//      {
//        uint8_t readData = 0;
//        if(myIMU.begin(sampleRate, 1, 1, 1, accelRange) == 0)
//        {
//          Serial.print("IMU initialized.\n");
//          delay(100);
//          myIMU.intConf(INT_1, DET_MOVE, 13, 0);
//          myIMU.readRegister(&readData, LIS3DH_INT1_CFG);
//          myIMU.readRegisterInt16( &sensor.x, LIS3DH_OUT_X_L );
//          myIMU.readRegisterInt16( &sensor.y, LIS3DH_OUT_Y_L );
//          myIMU.readRegisterInt16( &sensor.z, LIS3DH_OUT_Z_L );
//          mydata[i++] = (sensor.x>>8) & 0xFF;
//          mydata[i++] = (sensor.x)    & 0xFF;
//          mydata[i++] = (sensor.y>>8) & 0xFF;
//          mydata[i++] = (sensor.y)    & 0xFF;
//          mydata[i++] = (sensor.z>>8) & 0xFF;
//          mydata[i++] = (sensor.z)    & 0xFF;    
//        }
//        else
//          Serial.print("Failed to initialize IMU.\n");
//      }

      LMIC_setTxData2(sys.port, mydata, i, 1);
//      LoraWANPrintLMICOpmode();
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
      mydata[i++] = (devicet.firm_ver>>8)&0xff;
      mydata[i++] = devicet.firm_ver&0xff;
      mydata[i++] = devicet.freq_band;
      mydata[i++] = devicet.sub_band;
      mydata[i++] = (devicet.battrey>>8)&0xff;
      mydata[i++] = devicet.battrey&0xff;
      
      LMIC_setTxData2(sys.port, mydata, i,1);
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
#if defined(CFG_eu868)|| defined( CFG_as923)|| defined( CFG_kr920)|| defined( CFG_in866)
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
      delay(100); 
      digitalWrite(LED_PIN_RED, LOW);
      sys.alarm = 1;
    break;
    case ESP_SLEEP_WAKEUP_EXT1 : 
//    buzzer();
    sys.gps_start = 0;
    sys.alarm_count = 0;
    sys.alarm = 1;
    sys.exti_flag = 0;
    Serial.println("Wakeup caused by external signal using RTC_CNTL"); 
    break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default :
      sys.gps_alarm = 0;
      sys.gps_start = 1;
      sys.alarm = 0;
      sys.alarm_count =0;
      sys.port = 10;
      fcnt_flag = 1;
      sys.channel_single = 1;
      sys.sensor_mode =1;
      sys.config_Write();     
      Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); 
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
  LMIC_setClockError(MAX_CLOCK_ERROR * 20 / 100);
  #if defined( CFG_us915 ) || defined( CFG_au915 )
  LMIC_selectSubBand(sys.channel_single);
  #endif
  if (RTC_LMIC.seqnoUp != 0)
  {
    LMIC = RTC_LMIC;
  }
  Serial.printf("sys.alarm:%d\r\n",sys.alarm); 
  Serial.printf("gps_start:%d\r\n",sys.gps_start); 
  Serial.printf("gps_count:%d\r\n",sys.alarm_count); 
  Serial.printf("TDC:%d\r\n",sys.tdc); 
  Serial.printf("addr_gps_write:%d\r\n",sys.addr_gps_write);     
  sensor.bat = BatGet();
  if(sensor.bat<2840)
  {
    sys.gps_work_flag = false;
    sensor.latitude  = 0xFFFFFFFF;
    sensor.longitude = 0xFFFFFFFF;
    Serial.println("Low voltage,do not start GPS");
  }
  else if(sys.gps_start == 2 && sys.loggpsdata_send == 0)
  {
    GPS_Init();
    GPS_boot();
//    sys.gps_work_flag = false;
    sys.alarm_count =0;
    os_JOINED_flag = 0;
    Serial.println("Start searching for GPS...");  
//定时器
    timerSemaphore = xSemaphoreCreateBinary();
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, sys.Positioning_time*1000, true);
    timerAlarmEnable(timer);
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
  
  alarm_state();
  
  if (sys.gps_start == 1 )
  {
    device_start();
  }   
  
  gps_send();  
     
  sys_sleep();
}

void gps_send(void)
{
  if(sys.gps_work_flag == true )
  {
    if(GPS_DATA() == true)
    {
      os_JOINED_flag = 0;
      sys.gps_work_flag = false;
      timerAlarmDisable(timer);
    }
    else if(sys.loggpsdata_send == 1)
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
}

void sys_sleep(void)
{
  const bool timeCriticalJobs = os_queryTimeCriticalJobs(ms2osticksRound((sys.tdc)));
  if (!timeCriticalJobs && send_complete == true && !(LMIC.opmode & OP_TXRXPEND))
  {
    GXHT3x_LowPower();
    GPS_shutdown();
    myIMU.imu_power_down();
    myIMU.begin(sampleRate, 1, 1, 1, accelRange);      
//    LoraWANPrintLMICOpmode();
//    Serial.println();
    digitalWrite(BAT_PIN_LOW, LOW); 
    sys.config_Write();              
    if(sys.gps_start == 0)
    {   
      if(sys.alarm_count<=60)
      {  
        sys.alarm_count++;          
        sys.gps_start = 0;
        os_JOINED_flag = 0;
        if(sys.alarm_count == 60)
        {
          sys.alarm = 0;
          sys.gps_alarm = 0;
        }
        else
        {
          sys.alarm = 1;
          sys.gps_alarm = 1; 
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
    os_JOINED_flag = 1;
    Serial.println("Enter sleep mode");
    if(sys.loggpsdata_send == 0)
    {
      SaveLMICToRTC(sys.tdc);      
    }
    else
    {
      SaveLMICToRTC(10000);  
    }
    hal_sleep();    
    sys.LORA_EnterSleepMode();
    Serial.flush(); 
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,1); //1 = High, 0 = Low
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ALL_LOW ); //1 = High, 0 = Low
    if(sys.loggpsdata_send == 0)
    {
     esp_sleep_enable_timer_wakeup(sys.tdc*1000); //设置定时器唤醒 
    }
    else
    {
     esp_sleep_enable_timer_wakeup(10000*1000); //设置定时器唤醒
    }

    esp_deep_sleep_start(); //启动DeepSleep
  }
}
void alarm_state(void)
{
  if(sys.exti_flag == 1)
  {
    if(sys.gps_alarm == 1)
    {
      if(sys.buzzer_flag == 1)
      {
        buzzer();       
        delay(5000);
        Serial.println("stop ");
        Stop_buzzer();
        sys.buzzer_flag = 0;

      }
      else if(sys.buzzer_flag == 0)
      {

        Serial.printf("send NO.%d Alarm data \n\r",sys.alarm_count);    
        sys.gps_alarm =0 ;
        sys.buzzer_flag = 2;
        sys.exti_flag = 3;
      }
    }
    if(sys.alarm_count == 60)
    {
      sys.gps_alarm = 0;
      sys.gps_start = 2;
      sys.alarm = 0;
      sys.alarm_count =0;
      sys.exti_flag = 3;
//      sys.gps_work_flag = false;
      sys.config_Write(); 
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
      if (millis() - sleep_last > 15000)
      {
        sys.exti_flag = 2;
        sleep_last = millis();
      } 
    }     
  }
  else if (sys.exti_flag == 2)
  { 
    sys.gps_alarm = 0;
    sys.gps_start = 2;
    sys.alarm = 0;
    sys.alarm_count =0;
    sys.exti_flag = 3;
//      sys.gps_work_flag = false;
    sys.config_Write();       
    Serial.println("exti sleep mode");
    SaveLMICToRTC(sys.tdc);
    sys.LORA_EnterSleepMode();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,1); //1 = High, 0 = Low
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ALL_LOW ); //1 = High, 0 = Low
    esp_sleep_enable_timer_wakeup(sys.tdc*1000); //设置定时器唤醒
    esp_deep_sleep_start(); //启动DeepSleep 
  }
  else if (sys.exti_flag == 3)
  { 
    GPS_Init();
    GPS_boot();
    if(sys.alarm == 0)
    {
      Serial.println("Start searching for GPS..."); 
    } 
    sys.exti_flag = 4;
    timerSemaphore = xSemaphoreCreateBinary();
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 60000000, true);
    timerAlarmEnable(timer);
  }  
}
static void LORA_RxData(uint8_t *AppData, uint8_t AppData_Len)
{
  bool store_flag = false;
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
    case 0x22:
    {
      store_flag = true;
      if(( AppData_Len == 2 )&&(AppData[1]==0x01))   //---->AT+ADR=1
      {   
        LMIC_setAdrMode(1);
      }
      else if(( AppData_Len == 4 )&&(AppData[1]==0x00)) //---->AT+ADR=0
      {
        LMIC_setAdrMode(0);
        LMIC_setDrTxpow((AppData[2]<8)?AppData[2]:sys.LORA_GetDR(),
                        (AppData[3]!=0xFF)?AppData[3]:sys.LORA_GetTXP());
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
