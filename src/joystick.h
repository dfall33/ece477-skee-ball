#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__
#include "stm32f0xx.h"

void setup_adc(void);

void setup_tim16(void);

void enable_servo(void);

void disable_servo(void);

int map_adc_to_degrees(int adc_val);

void move_to_angle(int angle);

int read_joystick(void);

void TIM2_IRQHandler();

void init_tim2(void);

void enable_joystick(void);

void disable_joystick(void);



#endif