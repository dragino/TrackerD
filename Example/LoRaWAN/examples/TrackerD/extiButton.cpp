#include "extiButton.h"

//函数原形： OneButton(const int pin, const boolean activeLow = true, const bool pullupActive = true);
              /** * Initialize the OneButton library. * @param pin The pin to be used for input from a momentary button. * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true. * @param pullupActive Activate the internal pullup when available. Default is true. */
OneButton button(BUTTON_PIN,true,true);//
         //Instantiate a OneButton object
         //Three parameters can be passed in it:
         //pin : the pin corner of the button
         //activeLow : true: low level when pressed; false : high level when pressed; default value when not set: true
         //pullupActive : activate the pull-up resistor if there is a pull-up resistor
unsigned long SLEEP_START = 0UL;
// key event callback function
//click
void attachClick()
{
    sys.exti_flag = 2;
}

//Double Click
void attachDoubleClick()
{
//    Serial.println("doubleclick");
}

//Long Press Start
void attachLongPressStart()
{  
}
//During Long Press
void attachDuringLongPress()
{   
  if (button.isLongPressed())
  {
    sys.sleep_flag = 1;
    if(sys.lon == 1)
    {
      digitalWrite(LED_PIN_GREEN, HIGH);
      delay(1000);   
      digitalWrite(LED_PIN_GREEN, LOW);  
    }      
  }
}

//Long Press Stop
void attachLongPressStop()
{
}

//Multi Click
void attachMultiClick()
{   
  switch(button.getNumberClicks()){  
      case 3:
      {
        if(sys.sleep_flag == 0)               //Press three times to enter the alarm state
        {
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
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_BLUE, HIGH);
            delay(500);   
            digitalWrite(LED_PIN_BLUE, LOW);  
            delay(500);
            digitalWrite(LED_PIN_BLUE, HIGH);
            delay(500);   
            digitalWrite(LED_PIN_BLUE, LOW);  
            delay(500);
            digitalWrite(LED_PIN_BLUE, HIGH);
            delay(500);   
            digitalWrite(LED_PIN_BLUE, LOW);  
            delay(500);                          
          }          
        }
        else                                //After 10 seconds of long press, the device wakes up from sleep after three quick presses
        {
          sys.exti_flag = 2;
          sys.gps_alarm = 0;
          sys.gps_start = 2;
          sys.alarm = 0;
          sys.alarm_count =0;
          sys.config_Write();          
          sys.exti_flag = 3;
        }
        Serial.printf("switch3\r\n");
        break;
      }
      case 10:
      {
        if(sys.sleep_flag == 0)          //Press 10 times to wake the device from sleep
        {        
          sys.gps_alarm = 0;
          sys.gps_start = 2;
          sys.alarm = 0;
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
            digitalWrite(LED_PIN_RED, HIGH); 
            digitalWrite(LED_PIN_BLUE, HIGH);
            if (millis() - SLEEP_START > 5000)
            {
              digitalWrite(LED_PIN_RED, LOW);  
              digitalWrite(LED_PIN_BLUE, LOW);  
              SLEEP_START = millis();
            }    
            Serial.println("Exit Alarm");  
          }
        }
        else                            //After 10 seconds of long press, the device wakes up from sleep after 10 quick presses
        {  
          if(sys.lon == 1)
          {
            digitalWrite(LED_PIN_RED, HIGH);
//          if (millis() - SLEEP_START > 5000)
//          {
            delay(5000);
            digitalWrite(LED_PIN_RED, LOW);  
//            SLEEP_START = millis();
            sys.exti_flag = 2;
            sys.gps_alarm = 0;
            sys.gps_start = 2;
            sys.alarm = 0;
            sys.alarm_count =0;
            sys.config_Write();            
            Serial.printf("exti sleep\r\n");
//          }
          }           
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
  button.setDebounceTicks(80);//Set the debounce duration to 80 milliseconds, the default value: 50 milliseconds

  /** * set # millisec after single click is assumed. */
  //void setClickTicks(const int ticks);
  button.setClickTicks(500);//Set the click duration to 500 milliseconds, the default value: 400 milliseconds

  /** * set # millisec after press is assumed. */
  //void setPressTicks(const int ticks);
  button.setPressTicks(10000);//Set the long press duration to 10000 milliseconds, the default value is: 800 milliseconds
  
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
}
