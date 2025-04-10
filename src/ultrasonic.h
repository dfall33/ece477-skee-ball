#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include <stdint.h>
#include "stm32f0xx.h"

/* ---- Constants ----- */
#define HCSR04_PULSE_THRESHOLD_US 250
#define HCSR04_PULSE_TIMEOUT_US 30000
#define HCSR04_GAP_TIME_US 150
// #define HCSR04_GAP_TIME_US 1000 // 1ms gap between sensor readings
#define BALL_NOT_FOUND -1

/* ---- Function Prototypes ----- */
uint8_t read_hcsr04(int index);
void start_hcsr04_pulse_timer();
void stop_hcsr04_pulse_timer();
void start_hcsr04_search_timer();
void stop_hcsr04_search_timer();
void send_hcsr04_pulse(GPIO_TypeDef *port, uint32_t pin);
uint8_t wait_for_echo(GPIO_TypeDef *port, uint32_t pin, uint32_t odr_pin);
void time_out_pulse();
void time_out_hcsr04_search();
int search_hcsr04(int stability_count);

int test_sensor(int8_t index);
int test_sensors();

void setup_ultrasonic_ports();

void setup_tim14();
void setup_tim15();

#endif // __ULTRASONIC_H