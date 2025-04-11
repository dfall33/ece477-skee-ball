#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "stm32f0xx.h"

/* ----- Function Prototypes ----- */

/**
 * @brief Set the up debug GPIO ports 
 * 
 */
void setup_debug_ports();


/**
 * @brief Turn on the LED at the specified index (0-3)
 * 
 * @param index The index of the LED to turn on (0-3)
 */
void led_high(int8_t index);

/**
 * @brief Turn off the LED at the specified index (0-3)
 * 
 * @param index The index of the LED to turn off (0-3)
 */
void led_low(int8_t index);

/**
 * @brief Turn off all LEDs
 * 
 */
void led_off(); 

/**
 * @brief Flash the LEDs in sequence
 * 
 */
void flash_leds(); 

#endif // __DEBUG_H__