#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "stm32f0xx.h"

void setup_debug_ports();
void led_high(int8_t index);
void led_low(int8_t index);
void led_low(int8_t index);

void led_off(); 

#endif // __DEBUG_H__