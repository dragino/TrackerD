#ifndef _extiButton_h_
#define _extiButton_h_


#include <Arduino.h>
#include "OneButton.h"
#include "common.h"

#define BUTTON_PIN 0 

void attachClick();

void attachDoubleClick();

void attachLongPressStart();

void attachDuringLongPress();

void attachLongPressStop();

void attachMultiClick();

void button_event_init();

void button_attach_loop();

void alarm();

#endif
