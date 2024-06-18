#ifndef _blescan_h_
#define _blescan_h_

#include "common.h"


void ble_init();
void ble_run();
void ble_data(void);
int hexToint_ble(char *str);

#endif
