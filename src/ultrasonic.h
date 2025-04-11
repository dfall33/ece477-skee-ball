#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include <stdint.h>
#include "stm32f0xx.h"

/* ---- Constants ----- */
#define HCSR04_PULSE_THRESHOLD_US 250
#define HCSR04_PULSE_TIMEOUT_US 30000
#define HCSR04_GAP_TIME_US 500
#define BALL_NOT_FOUND -1

/* ---- Function Prototypes ----- */

/**
 * @brief Take a reading from the ultrasonic sensor and return the duration of the echo pulse in us 
 * 
 * @param index The index of the sensor to read from (0-3)
 * @return uint8_t  The duration of the echo pulse in us
 */
uint8_t read_hcsr04(int index);

/**
 * @brief Start the timer for the ultrasonic sensor pulse
 * 
 */
void start_hcsr04_pulse_timer();

/**
 * @brief Stop the timer for the ultrasonic sensor pulse
 * 
 */
void stop_hcsr04_pulse_timer();

/**
 * @brief Start the timer for the ultrasonic sensor search
 * 
 */
void start_hcsr04_search_timer();

/**
 * @brief Stop the timer for the ultrasonic sensor search
 * 
 */
void stop_hcsr04_search_timer();

/**
 * @brief Send a pulse to the ultrasonic sensor to trigger a reading
 * 
 * @param port The GPIO port to send the pulse to
 * @param pin The GPIO pin to send the pulse to
 */
void send_hcsr04_pulse(GPIO_TypeDef *port, uint32_t pin);

/**
 * @brief Wait for the echo from the ultrasonic sensor and return the duration of the echo pulse in us
 * 
 * @param port The GPIO port to read the echo from
 * @param pin The GPIO pin to read the echo from
 * @param odr_pin The GPIO pin to send the pulse to
 * @return uint8_t The duration of the echo pulse in us
 */
uint8_t wait_for_echo(GPIO_TypeDef *port, uint32_t pin, uint32_t odr_pin);

/**
 * @brief Handle the timeout for the ultrasonic sensor pulse
 * 
 */
void time_out_pulse();

/**
 * @brief Handle the timeout for the ultrasonic sensor search
 * 
 */
void time_out_hcsr04_search();

/**
 * @brief Search for the ball using the ultrasonic sensors and return the index of the sensor that detected the ball
 * 
 * @param stability_count The number of consecutive readings that must be stable before the ball is considered detected
 * @return int The index of the sensor that detected the ball, or -1 if the ball was not detected
 */
int search_hcsr04(int stability_count);

/**
 * @brief Test a single ultrasonic sensor and return the duration of the echo pulse in us
 * 
 * @param index The index of the sensor to test (0-3)
 * @return int The duration of the echo pulse in us, or 0 if the sensor did not detect anything
 */
int test_sensor(int8_t index);

/**
 * @brief Test all ultrasonic sensors and return the index of the sensor that detected the ball
 * 
 * @return int The index of the sensor that detected the ball, or -1 if the ball was not detected
 */
int test_sensors();

/**
 * @brief Set up the GPIO ports for the ultrasonic sensors
 * 
 */
void setup_ultrasonic_ports();


/**
 * @brief Set up the timer for the ultrasonic sensor pulse
 * 
 */
void setup_tim14();

/**
 * @brief Set up the timer for the ultrasonic sensor search
 * 
 */
void setup_tim15();

#endif // __ULTRASONIC_H