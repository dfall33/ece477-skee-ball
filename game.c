#include "game.h"

volatile int score = 0;
volatile int remaining_attempts = MAX_ATTEMPTS;

void game_idle()
{
    // enable interrupts for joystick and button
    enable_button_interrupt();

    // reset the score in between sessions
    score = 0;

    // reset the number of attempts between sessions
    remaining_attempts = MAX_ATTEMPTS;
}

void game_active()
{
    // enable interrupts for joystick and button
    enable_button_interrupt();
}

void game_button_press()
{

    /* ----- Leave only the button press interrupt enabled ----- */
    uint8_t press_level = get_press_duration(); // get the press duration from the button module (**BLOCKING**)

    // power the motor to launch the ball
    // power_motor(press_duration); // motor is not yet implemented

    // be done with the button for now
    disable_button_interrupt();

    // ball has been launched, so transition to the ball detection state
    game_ball_detection();
}

void game_ball_detection()
{

    // **BLOCKING** function that searches for the ball
    int sensor_index = search_hcsr04(1); // enables the necessary interrupts at the beginning and disables them at the end
    if (sensor_index != BALL_NOT_FOUND)  // -1 = not found, otherwise, gives index of the sensor that found the ball
    {
        score += SENSOR_SCORES[sensor_index]; // increment the score based on the sensor that found the ball
        // play_sound() // not yet implemented, Jen will do this
        // display_score(score); // update the score on the 4-line display, David will implement this
    }

    remaining_attempts--; // doing all this used one attempt, so decrement the number of attempts

    // transition to the next state, depending on how many attempts remain
    if (remaining_attempts == 0) // no more attempts, so go back to idle state
    {
        game_idle();
    }
    else // more attempts remain, so go back to active state
    {
        game_active();
    }
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