#include "extiButtonLS.h"
#include "lis3dh-motion-detection.h"
//函数原形： OneButton(const int pin, const boolean activeLow = true, const bool pullupActive = true);
              /** * Initialize the OneButton library. * @param pin The pin to be used for input from a momentary button. * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true. * @param pullupActive Activate the internal pullup when available. Default is true. */
OneButton button1(BUTTON_PIN1,false,false);//
         //Instantiate a OneButton object
         //Three parameters can be passed in it:
         //pin : the pin corner of the button
         //activeLow : true: low level when pressed; false : high level when pressed; default value when not set: true
         //pullupActive : activate the pull-up resistor if there is a pull-up resistor
unsigned long SLEEP_START1 = 0UL;
unsigned long ALARM_START1 = 0UL;
unsigned long ALARM_STOP1 = 0UL;
unsigned long ALARM_TIME1 = 0UL;
uint8_t operate_flag1 =0;
uint8_t button_count1 = 0;
#define EXIT_PIN_BITMASK 0x2000000
LIS3DH myIMU2(0x19); //Default address is 0x19.
// key event callback function
//click
void attachClick1()
{
  Serial.println("Button click.");
  operate_flag1 = 1;
  button_count1 ++;
}

//Double Click
void attachDoubleClick1()
{
//    Serial.println("doubleclick");
}

//Long Press Start
void attachLongPressStart1()
{  
  operate_flag1 = 0;
  ALARM_START1 = millis();
}
//During Long Press
void attachDuringLongPress1()
{ 
  if(sys.sensor_type == 13)
  {      
    sys.sleep_flag = 1;
    if(sys.lon == 1)
    {    
        digitalWrite(LED_PIN_GREEN, HIGH);
    }
    if (millis() - ALARM_START1 > sys.exit_alarm_time && millis() - ALARM_START1  < 10000)
    {
      digitalWrite(LED_PIN_GREEN, LOW);  
      sys.sleep_flag = 0;
      sys.alarm = 1;
      if(sys.lon == 1)
      {
        digitalWrite(LED_PIN_RED1, HIGH);
        digitalWrite(LED_PIN_GREEN, HIGH);
      }
    }
    else if (millis() - ALARM_START1 > 10000 && millis() - ALARM_START1  < 30000)
    {
      digitalWrite(LED_PIN_GREEN, LOW);  
      sys.sleep_flag = 1;
//      operate_flag = 1;
      if(sys.lon == 1)
      {
        digitalWrite(LED_PIN_RED1, HIGH);
        digitalWrite(LED_PIN_BLUE1, HIGH);
      }
//      digitalWrite(LED_PIN_GREEN, HIGH);      
    }
    else if(millis() - ALARM_START1 > 30000)
    {  
     digitalWrite(LED_PIN_GREEN, LOW);  
     digitalWrite(LED_PIN_BLUE1, LOW);
     if(sys.lon == 1)
     {  
        digitalWrite(LED_PIN_RED1, HIGH); 
     }
     sys.sleep_flag = 0;
     if(operate_flag1 == 1)
     {
        sys.exit_off =0;
        sys.alarm = 1;
        alarm(); 
        operate_flag1 = 0;
        sys.sleep_flag = 2;  
     }
    }
  }
  else if(sys.sensor_type == 22)
  {  
    if(sys.lon == 1)
    {    
        digitalWrite(LED_PIN_GREEN, HIGH);
    }
    if (millis() - ALARM_START1 > 5000 && millis() - ALARM_START1  < 10000)
    {
        digitalWrite(LED_PIN_GREEN, LOW);
        Serial.println("ALARM_START1");
        sys.sleep_flag = 1;  
        if(sys.lon == 1)
        {
          digitalWrite(LED_PIN_BLUE1, HIGH);
          digitalWrite(LED_PIN_GREEN, HIGH);
        }
    }     
  }
}

//Long Press Stop
void attachLongPressStop1()
{
  if(sys.sensor_type == 13)
  {
    ALARM_STOP1= millis();
    ALARM_TIME1 =  ALARM_STOP1- ALARM_START1;
    sys.exit_off = 1;
    Serial.println("attach Long Press Stop");
    if(sys.sleep_flag == 0)               //Press three times to enter the alarm state
    {
      digitalWrite(LED_PIN_RED1, LOW);
      digitalWrite(LED_PIN_GREEN, LOW);
      sys.alarm_count = 0;
      sys.alarm = 1;
      alarm1();  
    }
    else if (sys.sleep_flag == 1)
    {
     digitalWrite(LED_PIN_RED1, LOW);
     digitalWrite(LED_PIN_BLUE1, LOW);
     operate_flag1 = 1;
//     sys.alarm = 1;
     sys.sleep_flag = 1;
    }
  }
  else if(sys.sensor_type == 22)
  {
    ALARM_STOP1= millis();
    ALARM_TIME1 =  ALARM_STOP1- ALARM_START1;
    Serial.println("attach Long Press Stop1");
    if(sys.sleep_flag == 1)               //Press three times to enter the alarm state
    {
      digitalWrite(LED_PIN_BLUE1, LOW);
      digitalWrite(LED_PIN_GREEN, LOW);
      sys.sleep_exti =0;
      sys.config_Write();  
      ESP.restart(); 
    }    
  }
}

//Multi Click
void attachMultiClick1()
{
  if(sys.sensor_type == 13)
  {
    switch(button1.getNumberClicks()){  
      case 3:
      {
        if(sys.sleep_flag == 1)       //After 10 seconds of long press, the device wakes up from sleep after three quick presses
        { 
          operate_flag1 = 0;
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_RED1, HIGH);
          }
          if (millis() - SLEEP_START1 > 5000)
          {
            digitalWrite(LED_PIN_RED1, LOW);  
            SLEEP_START1 = millis();
          }   
          Serial.printf("SLEEP\r\n");          
          hal_sleep();    
          GXHT3x_LowPower();
          myIMU2.imu_power_down();  
          sys.LORA_EnterSleepMode();
          esp_deep_sleep_start(); //DeepSleep      
        }
        Serial.printf("switch:%d\r\n",button1.getNumberClicks());
        break;
      }      
      case 10:
      {  
        sys.gps_alarm = 0;
        sys.gps_start = 2;
        sys.alarm = 0;
        sys.keep_flag = 0;
        sys.alarm_count =0;
        sys.exti_flag = 4; 
        sys.gps_work_flag = false;
        if(sys.sensor_mode == 2 )
        {
          for(int i=0;i<45;i++)
          {
            sys.BLEDATA1[i]= {'f'};
          }   
        }          
        sys.config_Write(); 
        if(sys.lon == 1)
        {
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_BLUE1, HIGH);
          }
          if (millis() - SLEEP_START1 > 5000)
          {
            digitalWrite(LED_PIN_BLUE1, LOW);  
            SLEEP_START1 = millis();
          }    
          Serial.println("Exit Alarm");  
        }
        Serial.printf("switch:%d\r\n",button1.getNumberClicks());
        break;
      }
      default:{
//        sys.gps_alarm = 1;
//        sys.alarm = 1;
//        sys.exti_flag = 1;
//        sys.buzzer_flag = 0;
//        sys.alarm_no = 1;  
//        myIMU1.imu_power_down();   
        Serial.printf("switch:%d\r\n",button1.getNumberClicks());break;}
    }
  }
  else if(sys.sensor_type == 22)
  {
    switch(button1.getNumberClicks()){  
      case 5:
      { 
        if(sys.lon == 1)
        {
          digitalWrite(LED_PIN_RED1, HIGH);
          delay(5000);
          digitalWrite(LED_PIN_RED1, LOW); 
        }  
        Serial.printf("SLEEP\r\n"); 
        sys.sleep_exti =1;
        sys.config_Write();         
        myIMU2.imu_power_down();  
        GXHT3x_LowPower(); 
        GPS_shutdown();
        LMIC_shutdown(); 
        rtc_gpio_isolate(GPIO_NUM_27);//mosi 
        esp_sleep_enable_ext1_wakeup(EXIT_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH ); //1 = High, 0 = Low    
        sys.LORA_EnterSleepMode();
        Serial.end();      
        gpio_reset();
        esp_deep_sleep_start(); //DeepSleep  
        Serial.printf("switch:%d\r\n",button1.getNumberClicks());
        break;
      }
      default:{
        if(sys.sleep_exti ==1)
        { 
          Serial.printf("SLEEP\r\n"); 
          sys.sleep_exti =1;
          sys.config_Write();         
          myIMU2.imu_power_down();  
          GXHT3x_LowPower(); 
          GPS_shutdown();
          LMIC_shutdown(); 
          rtc_gpio_isolate(GPIO_NUM_27);//mosi 
          esp_sleep_enable_ext1_wakeup(EXIT_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH ); //1 = High, 0 = Low    
          sys.LORA_EnterSleepMode();
          Serial.end();      
          gpio_reset();
          esp_deep_sleep_start(); //DeepSleep  
          Serial.printf("switch:%d\r\n",button1.getNumberClicks());          
        }
        break;
      }
    } 
  }     
  
}

//回调函数绑定子程序
void button_event_init1(){
    

  button1.reset();//Clear the state of the button state machine
   /** * set # millisec after safe click is assumed. */
  //void setDebounceTicks(const int ticks);
//  button.setDebounceTicks(80);//Set the debounce duration to 80 milliseconds, the default value: 50 milliseconds

  /** * set # millisec after single click is assumed. */
  //void setClickTicks(const int ticks);
  button1.setClickTicks(500);//Set the click duration to 500 milliseconds, the default value: 400 milliseconds

  /** * set # millisec after press is assumed. */
  //void setPressTicks(const int ticks);
//  button.setPressTicks(10000);//Set the long press duration to 10000 milliseconds, the default value is: 800 milliseconds
  
//  button.attachClick(attachClick);
//  button.attachDoubleClick(attachDoubleClick);
  button1.attachLongPressStart(attachLongPressStart1);
  button1.attachDuringLongPress(attachDuringLongPress1);
  button1.attachLongPressStop(attachLongPressStop1);
  button1.attachMultiClick(attachMultiClick1);
}

//Button detection status subroutine
void button_attach_loop1(){    
    button1.tick();
  if(operate_flag1 == 1)
  {
//      Serial.println("operate_flag == 1");
     if(button_count1 == 3)
     {
       operate_flag1 = 0;  
     }
    if (millis() - ALARM_STOP1 > 5000 && millis() - ALARM_STOP1 <= 1000)
    {
//      Serial.println("operate_flag == 2");
      sys.sleep_flag =0;
      alarm1();
      operate_flag1 = 0; 
      ALARM_STOP1 = millis(); 
    }    
  }     
}

void alarm1()
{
  if(sys.sleep_flag == 0)               //Press three times to enter the alarm state
  {
    digitalWrite(LED_PIN_RED1, LOW);
    digitalWrite(LED_PIN_GREEN, LOW);
    if(sys.sensor_mode == 1)
    {
     Serial.println("Alarm for GPS...");
    }
    else if(sys.sensor_mode == 2)
    {
     Serial.println("Alarm for BLE..."); 
    }
    else if(sys.sensor_mode == 3)
    {
     Serial.println("Alarm for BLE and GPS..."); 
    }          
    sys.gps_alarm = 1;
    sys.alarm = 1;
    sys.exti_flag = 1;
    sys.keep_flag = 1;
    sys.buzzer_flag = 1;
    sys.alarm_no = 1;
    if(sys.lon == 1)
    {
      digitalWrite(LED_PIN_RED1, HIGH);
      delay(3000);   
      digitalWrite(LED_PIN_RED1, LOW);                           
    }          
  }
}
