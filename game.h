#ifndef __GAME_H__
#define __GAME_H__

/* ----- Inclusions ----- */
#include "stm32f0xx.h"
#include "button.h"
#include "I2S_STM32.h"
#include "nhd_0440az.h"
#include "ultrasonic.h"

/* ----- Game Constants ----- */
#define MAX_ATTEMPTS 3
const int8_t SENSOR_SCORES[5] = {5, 10, 15, 20, 25}; // placeholder score values, will almost certainly change

/* ----- Function Declarations ----- */

/**
 * @brief Function to initialize the game, the "main" function of the game
 *
 */
void game();

/**
 * @brief Transition into the idle state. Enabled / disables the proper interrupts and prepares for transition to other states.
 *
 */
void game_idle();

/**
 * @brief Transition into the active state. Enabled / disables the proper interrupts and prepares for transition to other states.
 *
 */
void game_active();

/**
 * @brief Transition into the ball detection state. Enabled / disables the proper interrupts and prepares for transition to other states.
 *
 */
void game_button_press();

/**
 * @brief Transition into the ball detection state. Enabled / disables the proper interrupts and prepares for transition to other states.
 *
 */
void game_ball_detection();

#endif // __GAME_H__#