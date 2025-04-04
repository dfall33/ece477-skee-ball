#include "game.h"

volatile int score = 0;
volatile int remaining_attempts = MAX_ATTEMPTS;

char score_str[20];    // string to hold the score for display
char success_str[20];  // string to hold the success message for display
char attempts_str[20]; // string to hold the attempts left for display
char mode_str[20];     // string to hold the mode for display

char final_score_str[40];

extern void led_high(int);
extern void led_low(int);
extern void led_off();
extern void setup_tim17();
extern void setup_tim16();
extern void power_motor();

extern void micro_wait(int);

void game_idle()
{
    // enable interrupts for joystick and button
    enable_button_interrupt();

    disable_joystick(); // disable joystick to prevent any movement while in idle state
    disable_servo();

    // reset the score in between sessions
    score = 0;

    // reset the number of attempts between sessions
    remaining_attempts = MAX_ATTEMPTS;

    spi_write_str(
        "Press to play",
        0);

    snprintf(attempts_str, sizeof(attempts_str), "Attempts: %d", remaining_attempts); // format the score string for display
    spi_write_str(attempts_str, 2);

    spi_write_str("Game mode = Idle", 1); // display the score on the top line of the display
}

void game_active()
{

    spi_write_str("Game mode = Active", 1);

    disable_button_interrupt();

    micro_wait(1000000);
    // enable interrupts for joystick and button
    enable_button_interrupt();

    setup_tim16();
    enable_joystick();
    enable_servo();

    snprintf(score_str, sizeof(score_str), "Score: %d", score); // format the score string for display
    spi_write_str(score_str, 0);
}

void game_button_press()
{

    spi_write_str("Game mode = Button Press", 1); // display the score on the top line of the display

    disable_joystick();
    disable_servo();

    /* ----- Leave only the button press interrupt enabled ----- */
    uint8_t press_level = get_press_duration(); // get the press duration from the button module (**BLOCKING**)

    char press_duration_str[20];                                                                 // string to hold the press duration for display
    snprintf(press_duration_str, sizeof(press_duration_str), "Press duration: %d", press_level); // format the press duration string for display
    spi_write_str(press_duration_str, 3);                                                        // display the press duration on the top line of the display

    power_motor(1);

    // be done with the button for now
    disable_button_interrupt();

    // ball has been launched, so transition to the ball detection state
    game_state = BALL_DETECTION; // go to ball detection state
}

void game_ball_detection()
{

    disable_joystick();
    disable_servo();

    disable_button_interrupt();                     // disable button interrupt to prevent any button presses during ball detection
    spi_write_str("Game mode = Ball Detection", 1); // display the score on the top line of the display

    // **BLOCKING** function that searches for the ball
    int sensor_index = search_hcsr04(1); // enables the necessary interrupts at the beginning and disables them at the end
    if (sensor_index != BALL_NOT_FOUND)  // -1 = not found, otherwise, gives index of the sensor that found the ball
    {
        // int additional_score = SENSOR_SCORES[sensor_index]; // get the score for the sensor that found the ball
        led_high(sensor_index);
        // int additional_score = 5;  // placeholder score value, will almost certainly change
        int additional_score = SENSOR_SCORES[sensor_index]; // get the score for the sensor that found the ball
        score += additional_score;                          // increment the score based on the sensor that found the ball
        play_sound();                                       // not yet implemented, Jen will do this
        // display_score(score); // update the score on the 4-line display, David will implement this

        snprintf(success_str, sizeof(success_str), "+%d points!", score); // format the score string for display
        spi_write_str(success_str, 1);                                    // display the score on the top line of the display
    }

    remaining_attempts--; // doing all this used one attempt, so decrement the number of attempts

    snprintf(attempts_str, sizeof(attempts_str), "Attempts: %d", remaining_attempts); // format the score string for display
    spi_write_str(attempts_str, 2);                                                   // display the score on the top line of the display

    // transition to the next state, depending on how many attempts remain
    if (remaining_attempts == 0) // no more attempts, so go back to idle state
    {
        show_final_score();
        game_state = IDLE; // go back to idle state
    }
    else // more attempts remain, so go back to active state
    {
        game_state = ACTIVE; // go back to active state
    }
}

void show_final_score()
{
    clear_display();
    snprintf(final_score_str, sizeof(final_score_str), "Final Score: %d", score); // format the score string for display
    spi_write_str("Game Over!", 1);                                               // display the score on the top line of the display
    spi_write_str(final_score_str, 2);                                            // display the score on the top line of the display
    micro_wait(1000000);
    micro_wait(1000000);
    micro_wait(1000000);
    micro_wait(1000000);
    micro_wait(1000000);
    micro_wait(1000000);
}

void game()
{

    while (1)
    {

        if (game_state == IDLE)
        {
            game_idle();
        }
        else if (game_state == ACTIVE)
        {
            game_active();
        }
        else if (game_state == BUTTON_PRESS)
        {
            game_button_press();
        }
        else if (game_state == BALL_DETECTION)
        {
            game_ball_detection();
        }
    }

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