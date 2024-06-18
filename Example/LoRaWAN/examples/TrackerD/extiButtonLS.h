#ifndef _extiButtonLS_h_
#define _extiButtonLS_h_


#include <Arduino.h>
#include "OneButton.h"
#include "common.h"

#define BUTTON_PIN1 25 

void attachClick1();

void attachDoubleClick1();

void attachLongPressStart1();

void attachDuringLongPress1();

void attachLongPressStop1();

void attachMultiClick1();

void button_event_init1();

void button_attach_loop1();

void alarm1();

#endif
