#ifndef __GAME_H__
#define __GAME_H__

/* ----- Inclusions ----- */
#include "stm32f0xx.h"
#include "button.h"
#include "I2S_STM32.h"
#include "nhd_0440az.h"
#include "ultrasonic.h"
#include <stdio.h>
#include "joystick.h"

/* ----- Game Constants ----- */
#define MAX_ATTEMPTS 3
int8_t SENSOR_SCORES[5] = {5, 10, 15, 20, 0}; // placeholder score values, will almost certainly change

/*

    State 1: Powered up or turn is ended: in idle state
        Enabled interrupts:
            1. For reading from joystick (poll joystick displacement and turn servo if necessary)
            2. For reading from push button (press and hold to launch ball)
            When either of these two interrupts are triggered, move to another state

    State 2: Reading from push button
        When the button is pressed down and its interrupt is triggered, all other interrupts should be disabled (nothing else
        in the game can happen until the ball has been launched).

        When the button is released (release is interpreted as a second trigger of the external button interrupt)
        and the motor launches the ball, we move to the ball detection state.

        Enabled interrupts:
            1. Button release interrupt (the same interrupt that got us here in the first place)

    State 3: Ball detection
        This state is entered immediately after the button is released and the motor launches the ball.
        In this state, everything is disabled: user cannot use the joystick or the button.

        This state will end in one of two ways:
            1. The ball is detected by the ultrasonic sensor, triggering another interrupt for recording the score and playing the sound.
            2. The ball is not detected and a timer that was started when this state was entered (TIM14 starts when button is released and has
            counter and scaler set so that it triggers an interrupt) triggers an interrupt to stop searching for the ball and go to the next state

        This state will transition to either State 1 (idle) if the user is out of turns, or to State 4 (active) if the user has more turns.

        Enabled interrupts:
            1. TIM14 (search timeout, i.e., the ball was not detected anywhere (user probably missed))
            2. Ball detection interrupt (play music and increment score based on which sensor detected the ball)


    State 4: Active state
        This state encompasses the time during which the user has already started to play and is not yet
        out of attempts, but the ball has not been launched yet.

        So the user is free to use the button and joystick to play the game, but nothing is "really happening" at the moment, so it is similar to State 1.

        Enabled interrupts:
            1. For reading from joystick (poll joystick displacement and turn servo if necessary)
            2. For reading from push button (press and hold to launch ball)
            When either of these two interrupts are triggered, move to another state

*/
typedef enum STATES
{
    IDLE,
    ACTIVE,
    BUTTON_PRESS,
    BALL_DETECTION
} STATES;

/* ---- Initialize Game State ----- */
volatile int game_state = IDLE;

/* ----- Function Declarations ----- */

/**
 * @brief Function to loop over game states and execute accordingly.
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

/**
 * @brief Display a string showing that the game is over and the final score.
 * 
 * @param score The final score to display.
 */
void show_final_score(); 

#endif // __GAME_H__#