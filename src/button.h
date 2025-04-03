#ifndef __BUTTON_H__
#define __BUTTON_H__
#include "stm32f0xx.h"
#define BUTTON_MAX_PRESS_US 5000 // 10s / 10000ms

// volatile int press_duration = -1;

/**
 * @brief Called by the button interrupt handler, this function will set or reset any necessary flags and start the timer to measure
 * how long the button is pressed.
 *
 */
void start_button_press();

/**
 * @brief Called by the button interrupt handler, this function will set or reset any necessary flags and stop the timer that measures
 * how long the button is pressed.
 *
 */
void stop_button_press();

/**
 * @brief Called by start_button_press, this function will start / enable the timer that measures how long the button is pressed.
 *
 */
void start_button_timer();

/**
 * @brief Called by stop_button_press, this function will stop / disable the timer that measures how long the button is pressed.
 *
 */
void stop_button_timer();

/**
 * @brief Called by the timer interrupt handler, this function will set the flag to indicate that the button press has timed out.
 *
 */
void time_out_button();

/**
 * @brief Disables the external interrupt associated with button press and release
 *
 */
void disable_button_interrupt();

/**
 * @brief Enables the external interrupt associated with button press and release
 *
 */
void enable_button_interrupt();

/**
 * @brief This function will block until the button press has timed out or the button has been released. It will return the duration
 * of the button press in microseconds.
 *
 * @return int The duration of the button press scaled to a range [1, 10]
 */
int get_press_duration();

/**
 * @brief Initializes the external interrupt associated with the button press and release (PC13)
 *
 */
void init_button_exti();

/**
 * @brief Initializes the GPIO associated with the button press and release (PC13)
 *
 */
void init_button_gpio();

/**
 * @brief TIM3 is used for timing how long the button is pressed. The prescaler is set such that
 * the timer counter represents the time pressed in microseconds. The timer has an interrupt so that the button can be constrained to a
 * maximum press time (defined in src/button.h)
 *
 */
void setup_tim3();

#endif // __BUTTON_H__