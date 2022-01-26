#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <stdbool.h>
#include "common.h"

#define maxRxSize       128 
#define EXTI_PIN        0

static bool send_complete = false;
static uint8_t serial_flag = AT_OK;
uint32_t time1;

static char rxData[maxRxSize]={0};
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
// Saves the LMIC structure during DeepSleep
RTC_DATA_ATTR lmic_t RTC_LMIC;

void os_getArtEui (u1_t* buf) { memcpy_P(buf, sys.appeui, 8);}

void os_getDevEui (u1_t* buf) { memcpy_P(buf, sys.deveui, 8);}

void os_getDevKey (u1_t* buf) {  memcpy_P(buf, sys.appkey, 16);}

static uint8_t mydata[50] = {0};
static osjob_t sendjob;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 23,
    .dio = {26, 33, 32},
};

void ARDUINO_ISR_ATTR onTimer(){
  // Give a semaphore that we can check in the loop
  //xSemaphoreGiveFromISR(timerSemaphore, NULL);
  serial_flag = AT_OK;
}

void onEvent (ev_t ev) {
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
            digitalWrite(LED_PIN_GREEN, HIGH);
            delay(1000);           
            Serial.println(F("EV_JOINED"));
            LMIC_getSessionKeys(&sys.netid, &sys.devaddr, sys.nwkSKey, sys.appSKey);
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
            // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
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
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.print(F("Received:"));
              for(int i=0;i<LMIC.dataLen;i++)
                Serial.printf("%.2X ",LMIC.frame[i+LMIC.dataBeg]);
              Serial.println();
              LORA_RxData(&LMIC.frame[LMIC.dataBeg],LMIC.dataLen);            
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
            digitalWrite(LED_PIN_GREEN, HIGH);
            delay(100);   
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
    digitalWrite(LED_PIN_RED, LOW);
    digitalWrite(LED_PIN_BLUE, LOW);
    digitalWrite(LED_PIN_GREEN, LOW);
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
      GXHT3x_GetData();
      GPS_INIT();
      GPS_DATA();
      Serial.print("Time:");  time1 = millis();
      Serial.println(time1);
//      if(wifi_scan()!=0)
//      {
//        memcpy(mydata,WiFi.BSSID(0),6);
//        memcpy(&mydata[6],WiFi.BSSID(1),6);
//        memcpy(&mydata[12],WiFi.BSSID(2),6);
//      }
      i=18;
      sensor.hum = (int)(GXHT3x_GetHum()*10);
      sensor.tem = (int)(GXHT3x_GetTem()*10);
      sensor.bat = BatGet();
      mydata[i++] = (sensor.bat>>8) & 0xFF;
      mydata[i++] = (sensor.bat)    & 0xFF;
      mydata[i++] = (sensor.hum>>8) & 0xFF;
      mydata[i++] = (sensor.hum)    & 0xFF;
      mydata[i++] = (sensor.tem>>8) & 0xFF;
      mydata[i++] = (sensor.tem)    & 0xFF;
      LMIC_setTxData2(2, mydata, i, 0);
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

    // EU Like Bands
#if defined(CFG_LMIC_EU_like)
    //Serial.println(F("Reset CFG_LMIC_EU_like band avail"));
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
    Serial.println(F("No DutyCycle recalculation function!"));
#endif
}

static void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : 
      Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); 
    break;
  }
}

void setup() {
  // put your setup code here, to run once:

  Wire.begin();
  Serial.begin(115200);
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM");
    return;
  }
  sys.config_Get();
  print_wakeup_reason();

  // This is necessary, early in the setup phase, because the ESP32-Arduino SPI
  // library has default pin assignement - that are NOT those of our board.
  // We override them here, and all subsequent uses of the SPI library will
  // use these.
  SPI.begin(LBT2_SCK, LBT2_MISO, LBT2_MOSI, LBT2_SS);
// LMIC init
  os_init();
// Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  if (RTC_LMIC.seqnoUp != 0)
  {
    LMIC = RTC_LMIC;
  }
// Start job (sending automatically starts OTAA too)
  do_send(&sendjob);
}

static uint8_t serial_index = 0;
void loop() {
  // put your main code here, to run repeatedly:
  uint8_t available_bytes = Serial.available();
  if (available_bytes > 0)//串口接收到数据
  {
    if(serial_flag == AT_OK)
    {
//定时器
      timerSemaphore = xSemaphoreCreateBinary();
      timer = timerBegin(0, 80, true);
      timerAttachInterrupt(timer, &onTimer, true);
      timerAlarmWrite(timer, 60000000, true);
      timerAlarmEnable(timer);
      serial_flag = AT_RUNNING;
    }
    else if(serial_flag == AT_RUNNING)
    {
      timerWrite(timer,0);
    }

    uint8_t end_of_string = false;

    // We read (at most) available_bytes Bytes from Serial port,
    // and store them in the buffer.
    for (uint8_t i=0; i < available_bytes; i++) {
      char result = (char)Serial.read();
      rxData[serial_index++] = result;

      // If the string is terminated, it's time to send it to the parser.
      if (result == '\n') {
        end_of_string = true;
        serial_index = 0;
        break;
      }

      // If we overflow the buffer, we stop and flush the input up to that point
      if (serial_index >= maxRxSize) {
        Serial.println(ATError_description[AT_TEST_PARAM_OVERFLOW]);
        serial_index = 0;
        memset(rxData,0,maxRxSize);
      }
    }

    if (end_of_string == true) {
      ATEerror_t AT_State = ATInsPro(rxData);
      memset(rxData,0,maxRxSize);
      if(AT_State == AT_OK) {
        Serial.println(ATError_description[AT_OK]);
      }
      else if(AT_State == AT_PARAM_ERROR) {
        Serial.println(ATError_description[AT_PARAM_ERROR]);
      }
      else if(AT_State == AT_BUSY_ERROR) {
        Serial.println(ATError_description[AT_BUSY_ERROR]);
      }
      else if(AT_State == AT_TEST_PARAM_OVERFLOW) {
        Serial.println(ATError_description[AT_TEST_PARAM_OVERFLOW]);
      }
      else if(AT_State == AT_RX_ERROR) {
        Serial.println(ATError_description[AT_RX_ERROR]);
      }
      else {
        Serial.println(ATError_description[AT_ERROR]);
      }
    }
  }
  os_runloop_once();

  const bool timeCriticalJobs = os_queryTimeCriticalJobs(ms2osticksRound((sys.tdc)));
  if (serial_flag != AT_RUNNING && !timeCriticalJobs && send_complete == true && !(LMIC.opmode & OP_TXRXPEND))
  {
    Serial.println("Enter sleep mode");
    SaveLMICToRTC(sys.tdc);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0,0); //1 = High, 0 = Low
    esp_sleep_enable_timer_wakeup(sys.tdc*1000); //设置定时器唤醒
    esp_deep_sleep_start(); //启动DeepSleep
  }
}
