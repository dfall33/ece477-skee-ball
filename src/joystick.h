#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__
#include "stm32f0xx.h"



void disable_joystick_interrupt(void);
void enable_joystick_interrupt(void);
void setup_joystick_exti(void);
void setup_adc(void);
void setup_tim16(void);
void init_tim2(void);
void move_to_angle(int angle);
int read_joystick(void);
int map_adc_to_degrees(int adc_val);


#endif