#include "stm32f0xx.h"
#include <stdio.h>

// PWM Parameters
#define PWM_PERIOD       37500  // Corresponding to 50Hz (20ms) with 48MHz clock

void setup_tim17(void);
void move_to_duty_cycle(int duty_cycle);

void setup_tim17(void) {
    // Enable GPIOB and Timer 17 clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;

    // Configure PB9 as alternate function for TIM17 Channel 1
    GPIOB->MODER &= ~GPIO_MODER_MODER9;
    GPIOB->MODER |= GPIO_MODER_MODER9_1;  // Set PB9 to alternate function mode
    GPIOB->AFR[1] &= GPIO_AFRH_AFRH1;  // Clear alternate function bits for PB9
    GPIOB->AFR[1] |= 2 << GPIO_AFRH_AFRH1_Pos;  // Set alternate function to TIM17_CH1

    // Configure TIM17 for PWM
    TIM17->PSC = 16 - 1;           // Prescaler to divide clock by 16
    TIM17->ARR = 10000 - 1;   // Set the auto-reload value for 50Hz
    TIM17->BDTR |= TIM_BDTR_MOE;   // Enable main output

    // PWM Mode 1 on Channel 1
    TIM17->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM17->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);  // PWM mode 1
    TIM17->CCMR1 |= TIM_CCMR1_OC1PE;           // Enable preload for CCR1

    // Enable output on TIM17 Channel 1
    TIM17->CCER |= TIM_CCER_CC1E;

    // Start the timer
    TIM17->EGR |= TIM_EGR_UG;       // Update the registers
    TIM17->CR1 |= TIM_CR1_CEN;      // Enable the timer
    TIM17->CCR1 = 800;  // Initialize CCR1 to 0
}

void move_to_duty_cycle(int duty_cycle) {
    // Ensure the duty cycle is within the PWM range (0 to 100)
    if (duty_cycle < 0) duty_cycle = 0;
    if (duty_cycle > 100) duty_cycle = 100;

    // Calculate the duty cycle in terms of the ARR and CCR1 values
    int pulse_width = 750 + (((duty_cycle - 1) * (1050 - 750)) / 9);
    TIM17->CCR1 = pulse_width;  // Set the duty cycle for PWM
    double high_time_ms = (double)pulse_width / 3000.0;
    printf("High time: %.2f ms\n", high_time_ms);
}

