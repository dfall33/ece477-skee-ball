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

    // game(); // start the game loop

    // return 0 ; 

    while (1)
    {
        int index = search_hcsr04(0); 
        if (index != -1)
        {
            char buffer[20];
            sprintf(buffer, "Sensor %d", index);
            spi_write_str(buffer, 0); // Display the sensor index on the display
            play_sound(); // play sound when the ball is detected
            // micro_wait(1000000); 
        }
        else
        {
            spi_write_str("No sensor", 0); // Display no sensor message on the display
        }
        micro_wait(1000000); 


    }

    // char buffer[20];
    // while (1)
    // {
    //     int duration = read_hcsr04(1); 
    //     sprintf(buffer, "Duration 0: %d", duration);
    //     spi_write_str(buffer, 0); // Display the duration on the display
    //     micro_wait(100000); 
    // }


    char buf1[20];
    char buf2[20];
    char buf3[20];
    char buf4[20];

    while (1)
    {
        // uint8_t duration0 = read_hcsr04(0); // front sensor
        // micro_wait(100000); 

        // uint8_t duration1 = read_hcsr04(1); // left sensor
        // micro_wait(100000); 

        // uint8_t duration2 = read_hcsr04(2); // right sensor
        // micro_wait(100000); 

        // uint8_t duration3 = read_hcsr04(3); // back sensor
        // micro_wait(100000); 

        // sprintf(buf1, "Duration 0: %d", duration0);
        // sprintf(buf2, "Duration 1: %d", duration1);
        // sprintf(buf3, "Duration 2: %d", duration2);
        // sprintf(buf4, "Duration 3: %d", duration3);

        // spi_write_str(buf1, 0); // Display the duration on the display
        // spi_write_str(buf2, 1); // Display the duration on the display
        // spi_write_str(buf3, 2); // Display the duration on the display
        // spi_write_str(buf4, 3); // Display the duration on the display

        // // micro_wait(1000000);
        // micro_wait(100000);

        char buf[20];
        for (int i = 0; i < 4; i++)
        {
            uint8_t duration = read_hcsr04(i); 

            if (duration > 0) 
            {
                snprintf(buf, sizeof(buf), "Sensor %d: %d", i, duration);
                spi_write_str(buf, i); // Display the duration on the display
            }
            // else
            // {
            //     snprintf(buf, sizeof(buf), "Sensor %d: No echo", i);
            //     spi_write_str(buf, i); // Display the duration on the display
            // }

            micro_wait(HCSR04_GAP_TIME_US); // wait for the gap time between sensor readings
        }
    }


    // while (1)
    // {
    //     spi_write_str("Checking duration", 1); 
    //     int duration = read_hcsr04(4); // front sensor 
    //     char buffer[20];
    //     sprintf(buffer, "Duration 4: %d", duration);
    //     spi_write_str(buffer, 0); // Display the duration on the display
    //     // micro_wait(1500); 
    //     micro_wait(100000);
        
    //     // duration = read_hcsr04(1); 
    //     // sprintf(buffer, "Duration 1: %d", duration);
    //     // spi_write_str(buffer, 2); // Display the duration on the display
    //     // micro_wait(100000);

    // }

    // power_motor(1);
    // move_to_duty_cycle(0);

    return 0;
}
