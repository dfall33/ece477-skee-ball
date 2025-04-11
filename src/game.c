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

volatile int idle_completed = 0; 
volatile int active_completed = 0;
volatile int button_press_completed = 0;
volatile int ball_detection_completed = 0;

void uncomplete_all() 
{
    idle_completed = 0;
    active_completed = 0;
    button_press_completed = 0;
    ball_detection_completed = 0;
}

void game_idle()
{

    uncomplete_all(); 
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

    idle_completed = 1; // set the idle state as completed

}

void game_active()
{

    uncomplete_all();

    snprintf(attempts_str, sizeof(attempts_str), "Attempts: %d", remaining_attempts); // format the score string for display
    spi_write_str(attempts_str, 2);

    disable_button_interrupt();

    micro_wait(1000000);
    // enable interrupts for joystick and button
    enable_button_interrupt();

    setup_tim16();
    enable_joystick();
    enable_servo();

    snprintf(score_str, sizeof(score_str), "Score: %d", score); // format the score string for display
    spi_write_str(score_str, 0);

    active_completed = 1; // set the active state as completed
}

void game_button_press()
{

    uncomplete_all();
    disable_joystick();
    disable_servo();

    /* ----- Leave only the button press interrupt enabled ----- */
    uint8_t press_level = get_press_duration(); // get the press duration from the button module (**BLOCKING**)

    power_motor(press_level); // power the motor based on the press level (0-10)
    disable_button_interrupt();
    game_state = BALL_DETECTION; // go to ball detection state
    button_press_completed = 1; // set the button press state as completed
}

void game_ball_detection()
{

    uncomplete_all();

    disable_joystick();
    disable_servo();

    disable_button_interrupt();                     // disable button interrupt to prevent any button presses during ball detection

    // **BLOCKING** function that searches for the ball
    int sensor_index = search_hcsr04(1); // enables the necessary interrupts at the beginning and disables them at the end
    if (sensor_index != BALL_NOT_FOUND)  // -1 = not found, otherwise, gives index of the sensor that found the ball
    {
        led_high(sensor_index);
        int additional_score = SENSOR_SCORES[sensor_index]; // get the score for the sensor that found the ball
        score += additional_score;                          // increment the score based on the sensor that found the ball
        play_sound();                                       // not yet implemented, Jen will do this
        snprintf(success_str, sizeof(success_str), "+%d points!", score); // format the score string for display
        spi_write_str(success_str, 1);                                    // display the score on the top line of the display
    }
    else
    {
        spi_write_str("+0 points!", 1); // display the score on the top line of the display
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

    ball_detection_completed = 1; // set the ball detection state as completed
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
        if (game_state == IDLE && idle_completed == 0)
        {
            game_idle();
        }
        else if (game_state == ACTIVE && active_completed == 0)
        {
            game_active();
        }
        else if (game_state == BUTTON_PRESS && button_press_completed == 0)
        {
            game_button_press();
        }
        else if (game_state == BALL_DETECTION && ball_detection_completed == 0)
        {
            game_ball_detection();
        }
    }

  
}