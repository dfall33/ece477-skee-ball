#ifndef __GAME_H__
#define __GAME_H__

#include "stm32f0xx.h"

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