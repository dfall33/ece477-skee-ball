#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

/* ----- Function Prototypes ----- */

/**
 * @brief Set up timer 17 (controls PWM for DC motor control)
 * 
 */
void setup_tim17(void);


/**
 * @brief Change PWM duty cycle 
 * 
 * @param duty_cycle integer [0, 10]
 */
void move_to_duty_cycle(int duty_cycle);

/**
 * @brief Enable the DC motor
 * 
 */
void enable_dc_motor();

/**
 * @brief Disable the DC motor
 * 
 */
void disable_dc_motor();

/**
 * @brief Disable the timer for PWM output
 * 
 */
void disable_tim17();

/**
 * @brief Drive the motor for 2 seconds then turn it off (wrapper on move_to_duty_cycle)
 * 
 * @param amount integer [0, 10]
 */
void power_motor(int amount);

#endif // __MOTOR_CONTROL_H__