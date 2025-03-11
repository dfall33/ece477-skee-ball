#include "game.h"

void game_idle()
{
    // enable interrupts for joystick and button
    // disable interrupts for ball detection
}

void game_active()
{
    // enable interrupts for joystick and button
    // disable interrupts for ball detection
}

void game_button_press()
{
    // leave only the button release interrupt enabled
}

void game_ball_detection()
{
    // enable interrupts for tim14 / search timeout and ball detection
    // disable the others
}

void game()
{

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
}