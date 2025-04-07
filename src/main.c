#include "stm32f0xx.h"
#include <math.h> // for M_PI
#include <stdint.h>
#include <stdio.h>
#include "ultrasonic.h"
#include "button.h"
#include "nhd_0440az.h"
#include "motor_control.h"
#include "joystick.h"
#include "debug.h"
extern game();
extern game_state;

#define PULSE_TIMEOUT_US 30000
#define PULSE_THRESHOLD_US 200

extern void setup_tim17();
extern int play_sound();

volatile int detected_streak = 0;

/* ----- Function Declarations ----- */
void nano_wait(int);
void micro_wait(int);

void setup_tim3();
void setup_tim14();
void setup_tim15();

int main(void)
{
    internal_clock();

    /* ----- Setup debugging LEDs ----- */
    setup_debug_ports();
    flash_leds();

    /* ----- Setup LCD Display ----- */
    setup_display(); // initialize SPI, GPIO ports for display
    test_display();  // display some text to show that the display is working correctly

    /* ----- Setup joystick and servo ----- */
    setup_adc();   // setup ADC for joystick input
    setup_tim16(); // setup Timer 16 for PWM output to the servo motor
    init_tim2();   // setup Timer 16 for joystick input polling

    /* ----- Setup external push button ----- */
    init_button_gpio(); // setup GPIO port for external button (PB7)
    init_button_exti(); // setup external interrupt for button press on PB7
    setup_tim3();       // setup Timer 3, the timer used for recording button press duration

    /* ----- Setup ultrasonic sensors ----- */
    setup_ultrasonic_ports(); // setup GPIO ports for ultrasonic sensors (HC-SR04)
    setup_tim14();            // setup Timer 14 for timing out individual ultrasonic sensor readings
    setup_tim15();            // setup Timer 15 for timing out the overall ultrasonic search (i.e., if the ball is not found within a certain time limit)

    // while (1)
    // {
    //     int duration = read_hcsr04(1);

    //     if (duration > 0)
    //     {
    //         led_high(1);
    //         play_sound();
    //     }
    //     else
    //     {
    //         led_low(1);
    //     }

    //     micro_wait(100000);
    // }

    // return 0;

    // while (1)
    // {
    //     // int index = search_hcsr04(1);

    //     int index = search_hcsr04(1);
    //     led_high(0);
    //     micro_wait(1000000);
    //     led_low(0);
    //     micro_wait(1000000); // wait for 1 second before the next search

    //     // if (index == -1)
    //     // {
    //     //     flash_leds();
    //     //     led_high(1);
    //     //     led_high(2);
    //     //     led_high(3);
    //     //     led_high(0);
    //     //     break;
    //     // }
    //     // else if (index == 3 || index == 4)
    //     // {
    //     //     led_high(3);
    //     //     // play_sound();
    //     // }
    //     // else if (index == 0)
    //     // {
    //     //     led_high(0);
    //     //     led_high(1);
    //     //     led_high(2);
    //     //     play_sound();
    //     // }
    //     // else
    //     // {
    //     //     led_high(index);
    //     // }

    //     // micro_wait(100000);
    //     // led_off();

    //     // micro_wait(1000000);
    // }
    game(); // start the game loop

    return 0;
}
