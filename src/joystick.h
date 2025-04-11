#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__
#include "stm32f0xx.h"

/* ----- Function Prototypes ----- */


/**
 * @brief Set up the adc 
 * 
 */
void setup_adc(void);

/**
 * @brief Set up timer 16 
 * 
 */
void setup_tim16(void);

/**
 * @brief Enable servo movement 
 * 
 */
void enable_servo(void);

/**
 * @brief Disable servo movement 
 * 
 */
void disable_servo(void);

/**
 * @brief Map ADC reading to angular displacement in degrees
 * 
 */
int map_adc_to_degrees(int adc_val);

/**
 * @brief Move servo to specified angle
 * 
 * @param angle The angle to move the servo to
 */
void move_to_angle(int angle);

/**
 * @brief Read joystick ADC value
 * 
 * @return int The ADC value of the joystick
 */
int read_joystick(void);

/**
 * @brief Poll the ADC  
 * 
 */
void TIM2_IRQHandler();

/**
 * @brief Initialize the timer for joystick polling
 * 
 */
void init_tim2(void);

/**
 * @brief Enable joystick movement 
 * 
 */
void enable_joystick(void);

/**
 * @brief Disable joystick movement 
 * 
 */
void disable_joystick(void);


#endif