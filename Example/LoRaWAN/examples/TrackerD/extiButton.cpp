#include "extiButton.h"
#include "lis3dh-motion-detection.h"
//函数原形： OneButton(const int pin, const boolean activeLow = true, const bool pullupActive = true);
              /** * Initialize the OneButton library. * @param pin The pin to be used for input from a momentary button. * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true. * @param pullupActive Activate the internal pullup when available. Default is true. */
OneButton button(BUTTON_PIN,true,true);//
         //Instantiate a OneButton object
         //Three parameters can be passed in it:
         //pin : the pin corner of the button
         //activeLow : true: low level when pressed; false : high level when pressed; default value when not set: true
         //pullupActive : activate the pull-up resistor if there is a pull-up resistor
unsigned long SLEEP_START = 0UL;
unsigned long ALARM_START = 0UL;
unsigned long ALARM_STOP = 0UL;
unsigned long ALARM_TIME = 0UL;
uint8_t operate_flag =0;
uint8_t button_count = 0;
LIS3DH myIMU1(0x19); //Default address is 0x19.
// key event callback function
//click
void attachClick()
{
  Serial.println("Button click.");
  operate_flag = 1;
  button_count ++;
}

//Double Click
void attachDoubleClick()
{
//    Serial.println("doubleclick");
}

//Long Press Start
void attachLongPressStart()
{  
  operate_flag = 0;
  ALARM_START = millis();
}
//During Long Press
void attachDuringLongPress()
{   
    sys.sleep_flag = 1;
    if(sys.lon == 1)
    {    
        digitalWrite(LED_PIN_GREEN, HIGH);
    }
    if (millis() - ALARM_START > 5000 && millis() - ALARM_START  < 10000)
    {
      digitalWrite(LED_PIN_GREEN, LOW);  
      sys.sleep_flag = 0;
      if(sys.lon == 1)
      {
        digitalWrite(LED_PIN_RED, HIGH);
        digitalWrite(LED_PIN_GREEN, HIGH);
      }
    }
    else if (millis() - ALARM_START > 10000 && millis() - ALARM_START  < 30000)
    {
      digitalWrite(LED_PIN_GREEN, LOW);  
      sys.sleep_flag = 1;
      operate_flag = 1;
      if(sys.lon == 1)
      {
        digitalWrite(LED_PIN_RED, HIGH);
        digitalWrite(LED_PIN_BLUE, HIGH);
      }
//      digitalWrite(LED_PIN_GREEN, HIGH);      
    }
    else if(millis() - ALARM_START > 30000)
    {  
     digitalWrite(LED_PIN_GREEN, LOW);  
     digitalWrite(LED_PIN_BLUE, LOW);
     if(sys.lon == 1)
     {  
        digitalWrite(LED_PIN_RED, HIGH); 
     }
     sys.sleep_flag = 0;
     if(operate_flag == 1)
     {
        sys.exit_off =0;
        alarm(); 
        operate_flag = 0;
        sys.sleep_flag = 2;  
     }
    }  
}

//Long Press Stop
void attachLongPressStop()
{
    ALARM_STOP= millis();
    ALARM_TIME =  ALARM_STOP- ALARM_START;
    sys.exit_off = 1;
    Serial.println("attach Long Press Stop");
    if(sys.sleep_flag == 0)               //Press three times to enter the alarm state
    {
      digitalWrite(LED_PIN_RED, LOW);
      digitalWrite(LED_PIN_GREEN, LOW);
      alarm();  
    }
    else if (sys.sleep_flag == 1)
    {
     digitalWrite(LED_PIN_RED, LOW);
     digitalWrite(LED_PIN_BLUE, LOW);
     operate_flag = 1;
     sys.sleep_flag = 1;
    }
}

//Multi Click
void attachMultiClick()
{   
  switch(button.getNumberClicks()){  
      case 3:
      {
        if(sys.sleep_flag == 1)       //After 10 seconds of long press, the device wakes up from sleep after three quick presses
        {
          
          operate_flag = 0;
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_RED, HIGH);
          }
          if (millis() - SLEEP_START > 5000)
          {
            digitalWrite(LED_PIN_RED, LOW);  
            SLEEP_START = millis();
          }   
          Serial.printf("SLEEP\r\n");          
          hal_sleep();    
          GXHT3x_LowPower();
          myIMU1.imu_power_down();  
          sys.LORA_EnterSleepMode();
          esp_deep_sleep_start(); //DeepSleep  
          
        }
        Serial.printf("switch3\r\n");
        break;
      }
      case 10:
      {  
        sys.gps_alarm = 0;
        sys.gps_start = 2;
        sys.alarm = 0;
        sys.keep_flag = 0;
        sys.alarm_count =0;
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
            digitalWrite(LED_PIN_BLUE, HIGH);
          }
          if (millis() - SLEEP_START > 5000)
          {
            digitalWrite(LED_PIN_BLUE, LOW);  
            SLEEP_START = millis();
          }    
          Serial.println("Exit Alarm");  
        }
        Serial.printf("switch10\r\n");
        break;
      }
      default:{
    sys.alarm = 0;
    sys.exti_flag = 2;
    Serial.printf("switch\r\n",button.getNumberClicks());break;}
  }
}

//回调函数绑定子程序
void button_event_init(){
    

  button.reset();//Clear the state of the button state machine
   /** * set # millisec after safe click is assumed. */
  //void setDebounceTicks(const int ticks);
//  button.setDebounceTicks(80);//Set the debounce duration to 80 milliseconds, the default value: 50 milliseconds

  /** * set # millisec after single click is assumed. */
  //void setClickTicks(const int ticks);
  button.setClickTicks(500);//Set the click duration to 500 milliseconds, the default value: 400 milliseconds

  /** * set # millisec after press is assumed. */
  //void setPressTicks(const int ticks);
//  button.setPressTicks(10000);//Set the long press duration to 10000 milliseconds, the default value is: 800 milliseconds
  
//  button.attachClick(attachClick);
//  button.attachDoubleClick(attachDoubleClick);
  button.attachLongPressStart(attachLongPressStart);
  button.attachDuringLongPress(attachDuringLongPress);
  button.attachLongPressStop(attachLongPressStop);
  button.attachMultiClick(attachMultiClick);
}

//Button detection status subroutine
void button_attach_loop(){    
    button.tick();
  if(operate_flag == 1)
  {
//      Serial.println("operate_flag == 1");
     if(button_count == 3)
     {
       operate_flag = 0;  
     }
    if (millis() - ALARM_STOP > 3000 && millis() - ALARM_STOP <= 4000)
    {
//      Serial.println("operate_flag == 2");
      sys.sleep_flag =0;
      alarm();
      operate_flag = 0; 
      ALARM_STOP = millis(); 
    }    
  }     
}

void alarm()
{
  if(sys.sleep_flag == 0)               //Press three times to enter the alarm state
  {
    digitalWrite(LED_PIN_RED, LOW);
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
    sys.buzzer_flag = 1;
    sys.alarm_no = 1;
    if(sys.lon == 1)
    {
      digitalWrite(LED_PIN_RED, HIGH);
      delay(3000);   
      digitalWrite(LED_PIN_RED, LOW);                           
    }          
  }
}
