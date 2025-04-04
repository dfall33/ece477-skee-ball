#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

void setup_tim17(void);
void move_to_duty_cycle(int duty_cycle);
void enable_dc_motor();
void disable_dc_motor();
void disable_tim17();

void power_motor(int amount);

#endif // __MOTOR_CONTROL_H__