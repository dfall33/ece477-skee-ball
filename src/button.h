#ifndef __BUTTON_H__
#define __BUTTON_H__
#include "stm32f0xx.h"
#define BUTTON_MAX_PRESS_US 5000 // 10s / 10000ms
void start_button_press();
void stop_button_press();
void start_button_timer();
void stop_button_timer();
void time_out_button();
int get_press_duration();
#endif // __BUTTON_H__