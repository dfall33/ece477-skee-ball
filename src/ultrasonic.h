#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include <stdint.h>

/* ---- Constants ----- */
#define HCSR04_PULSE_THRESHOLD_US 250
#define HCSR04_PULSE_TIMEOUT_US 30000
#define HCSR04_SEARCH_TIMEOUT_US 10000000
#define HCSR04_GAP_TIME_US 250
#define BALL_NOT_FOUND -1

/* ---- Function Prototypes ----- */
int read_hcsr04(int index);
void pulse_hcsr04_trigger();
void start_hcsr04_pulse_timer();
void stop_hcsr04_pulse_timer();
void start_hcsr04_search_timer();
void stop_hcsr04_search_timer();
void send_hcsr04_pulse(uint32_t pin);
int wait_for_echo(volatile uint32_t pin, uint32_t odr_pin);
void time_out_pulse();
void time_out_hcsr04_search();
int search_hcsr04(int stability_count);
#endif // __ULTRASONIC_H