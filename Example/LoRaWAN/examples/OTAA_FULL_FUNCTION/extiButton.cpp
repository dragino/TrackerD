#include "extiButton.h"

//函数原形： OneButton(const int pin, const boolean activeLow = true, const bool pullupActive = true);
              /** * Initialize the OneButton library. * @param pin The pin to be used for input from a momentary button. * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true. * @param pullupActive Activate the internal pullup when available. Default is true. */
OneButton button(BUTTON_PIN,true);//
        //实例化一个OneButton对象
        //里面可以传三个参数:
        //pin : 按钮的pin角
        //activeLow : true:按下为低电平; false : 按下为高电平;不设置时默认值为：true
        //pullupActive : 如果有上拉电阻就激活上拉电阻
unsigned long SLEEP_START = 0UL;
//按键事件回调函数
//单击
void attachClick()
{
    sys.exti_flag = 2;
//    Serial.println("click");
}

//双击
void attachDoubleClick()
{
//    Serial.println("doubleclick");
}

//长铵开始
void attachLongPressStart()
{  
//    Serial.println("longPressStart");
}
//长按过程
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
//    Serial.println("duringLongPress");
  }
}

//长按结束
void attachLongPressStop()
{
//    Serial.println("longPressStop");
}

//按下多次
void attachMultiClick()
{
    
  Serial.printf("getNumberClicks：%d\r\n",button.getNumberClicks());
  switch(button.getNumberClicks()){  
      case 3:
      {
        if(sys.sleep_flag == 0)
        {
          Serial.println("Alarm for GPS...");
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
        else
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
        if(sys.sleep_flag == 0)
        {        
          sys.gps_alarm = 0;
          sys.gps_start = 2;
          sys.alarm = 0;
          sys.alarm_count =0;
          sys.gps_work_flag = false;
          sys.config_Write(); 
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
        else
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
        Serial.printf("switch10\r\n");
        break;
      }
      default:{
    sys.alarm = 0;
    sys.exti_flag = 2;
    Serial.printf("switch7\r\n",button.getNumberClicks());break;}
  }
}

//回调函数绑定子程序
void button_event_init(){
    

  button.reset();//清除一下按钮状态机的状态
   /** * set # millisec after safe click is assumed. */
  //void setDebounceTicks(const int ticks);
  button.setDebounceTicks(80);//设置消抖时长为80毫秒,默认值为：50毫秒

  /** * set # millisec after single click is assumed. */
  //void setClickTicks(const int ticks);
  button.setClickTicks(500);//设置单击时长为500毫秒,默认值为：400毫秒

  /** * set # millisec after press is assumed. */
  //void setPressTicks(const int ticks);
  button.setPressTicks(10000);//设置长按时长为10000毫秒,默认值为：800毫秒
  
//  button.attachClick(attachClick);//初始化单击回调函数
//  button.attachDoubleClick(attachDoubleClick);//初始化双击回调函数
  button.attachLongPressStart(attachLongPressStart);//初始化长按开始回调函数
  button.attachDuringLongPress(attachDuringLongPress);//初始化长按期间回调函数
  button.attachLongPressStop(attachLongPressStop);//初始化长按结束回调函数
  button.attachMultiClick(attachMultiClick);//初始化按了多次(3次或以上)回调函数
}

//按钮检测状态子程序
void button_attach_loop(){
    
    //不断检测按钮按下状态
    button.tick();
}
