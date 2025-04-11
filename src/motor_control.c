#include "stm32f0xx.h"
#include <stdio.h>
#include "motor_control.h"

// PWM Parameters
#define PWM_PERIOD 37500 // Corresponding to 50Hz (20ms) with 48MHz clock

void setup_tim17(void);
void move_to_duty_cycle(int duty_cycle);

extern void micro_wait(int);

void setup_tim17(void)
{
    // Enable GPIOB and Timer 17 clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;

    // Configure PB9 as alternate function for TIM17 Channel 1
    GPIOB->MODER &= ~GPIO_MODER_MODER9;
    GPIOB->MODER |= GPIO_MODER_MODER9_1;       // Set PB9 to alternate function mode
    GPIOB->AFR[1] &= GPIO_AFRH_AFRH1;          // Clear alternate function bits for PB9
    GPIOB->AFR[1] |= 2 << GPIO_AFRH_AFRH1_Pos; // Set alternate function to TIM17_CH1

    // Configure TIM17 for PWM
    TIM17->PSC = 96 - 1;         // Prescaler to divide clock by 16
    TIM17->ARR = 10000 - 1;      // Set the auto-reload value for 50Hz
    TIM17->BDTR |= TIM_BDTR_MOE; // Enable main output

    // PWM Mode 1 on Channel 1
    TIM17->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM17->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos); // PWM mode 1
    TIM17->CCMR1 |= TIM_CCMR1_OC1PE;           // Enable preload for CCR1

    // Enable output on TIM17 Channel 1
    TIM17->CCER |= TIM_CCER_CC1E;

    // Start the timer
    TIM17->EGR |= TIM_EGR_UG; // Update the registers
    TIM17->CCR1 = 800;        // Initialize CCR1 to 0

    TIM17->CR1 |= TIM_CR1_CEN; // Enable the timer
}

void disable_tim17(void)
{

    // disable clock to TIM17
    TIM17->CR1 &= ~TIM_CR1_CEN; // Disable the timer to stop PWM output

    // disable MOE (Main Output Enable)
    TIM17->BDTR &= ~TIM_BDTR_MOE; // Disable main output

    TIM17->CCER &= ~TIM_CCER_CC1E; // Disable output on TIM17 Channel 1
    TIM17->CCR1 = 0;               // Set duty cycle to 0 to stop the motor

    // disable update generation
    TIM17->EGR &= ~TIM_EGR_UG; // Clear the update generation flag

    // reset output compare mode
    TIM17->CCMR1 &= ~TIM_CCMR1_OC1M;      // Clear the output compare mode bits
    RCC->APB2ENR &= ~RCC_APB2ENR_TIM17EN; // Disable the clock for TIM17
}

void power_motor(int amount)
{
    setup_tim17();
    move_to_duty_cycle(amount); // Set the duty cycle based on the amount (0-100%)
    micro_wait(1000000);
    micro_wait(1000000);
    
    move_to_duty_cycle(0); // Stop the motor after 1 second
    disable_tim17();
}

void move_to_duty_cycle(int duty_cycle)
{

    setup_tim17();
    if (duty_cycle < 0)
        duty_cycle = 0;
    if (duty_cycle > 10)
        duty_cycle = 10;

    // enforce a lower bound of 3 for the press duration, 1 and 2 are very weak (makes for better UI)
    if (duty_cycle == 1 || duty_cycle == 2 ) 
        duty_cycle = 3; 

    // linearly interpolate a number [0, 10] to [750, 820]
    int pulse_width = 750 + ((duty_cycle * (820 - 750)) / 10);
    TIM17->CCR1 = pulse_width; // Set the duty cycle for PWM
}

void enable_dc_motor()
{
    TIM17->CR1 |= TIM_CR1_CEN; // Enable the timer to start PWM output
}

void disable_dc_motor()
{
    TIM17->CR1 &= ~TIM_CR1_CEN; // Disable the timer to stop PWM output
}