// #include "stm32f0xx.h"
// #include <stdio.h>

// // Servo Parameters
// #define SERVO_MIN_PULSE 500  // 500us
// #define SERVO_MAX_PULSE 2500 // 2500us
// #define PWM_PERIOD 2400      // Corresponding to 50Hz (20ms) with 48MHz clock

// int degrees = 45;

// void setup_adc(void);
// void setup_tim16(void);
// void init_tim15(void);
// void move_to_angle(int angle);
// int read_joystick(void);
// int map_adc_to_degrees(int adc_val);

// // Setup ADC for Joystick Input (PA1)
// void setup_adc(void)
// {
//     RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//     GPIOA->MODER |= 0xC;
//     RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
//     RCC->CR2 |= RCC_CR2_HSI14ON;
//     while (!(RCC->CR2 & RCC_CR2_HSI14RDY))
//         ;
//     ADC1->CR |= ADC_CR_ADEN;
//     while (!(ADC1->ISR & ADC_ISR_ADRDY))
//         ;
//     ADC1->CHSELR |= ADC_CHSELR_CHSEL1;
//     while (!(ADC1->ISR & ADC_ISR_ADRDY))
//         ;
// }

// // Setup PWM on TIM16 Channel 1 (PB8)
// void setup_tim16(void)
// {
//     RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
//     RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

//     GPIOB->MODER |= 0x20000;
//     GPIOB->AFR[1] |= 0x2;

//     TIM16->PSC = 48 - 1;
//     TIM16->ARR = PWM_PERIOD - 1;

//     TIM16->CCMR1 |= 0x60;
//     TIM16->CCER |= TIM_CCER_CC1E;

//     TIM16->CR1 |= TIM_CR1_CEN;

//     move_to_angle(degrees);
// }

// // Convert ADC Value to Degree
// int map_adc_to_degrees(int adc_val)
// {
//     int adc_min = 0, adc_max = 4095;
//     int deg_min = -4, deg_max = 4;

//     return ((adc_val - adc_min) * (deg_max - deg_min) / (adc_max - adc_min)) + deg_min;
// }

// // Move Servo to Specified Angle
// void move_to_angle(int angle)
// {
//     if (angle < 0)
//         angle = 0;
//     if (angle > 180)
//         angle = 180;

//     int pulse_width = SERVO_MIN_PULSE + (angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE) / 180);
//     TIM16->CCR1 = pulse_width; // Set duty cycle
//     printf("Moving to %d degrees (Pulse: %d)\n", angle, pulse_width);
// }

// // Read Joystick ADC Value
// int read_joystick(void)
// {
//     ADC1->CR |= ADC_CR_ADSTART;
//     while (!(ADC1->ISR & ADC_ISR_EOC))
//         ;
//     return ADC1->DR;
// }

// // Timer 15 ISR
// void TIM15_IRQHandler()
// {
//     TIM15->SR &= ~TIM_SR_UIF;

//     int adc_val = read_joystick();
//     int inc_degrees = map_adc_to_degrees(adc_val);

//     if ((degrees + inc_degrees) >= 0 && (degrees + inc_degrees) <= 180)
//     {
//         degrees += inc_degrees;
//     }

//     move_to_angle(degrees);
//     printf("ADC: %d  Degrees: %d  Increment: %d\n", adc_val, degrees, inc_degrees);
// }

// // Setup TIM15 for Periodic Joystick Reading
// void init_tim15(void)
// {
//     RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
//     TIM15->PSC = 480 - 1;
//     TIM15->ARR = 100 - 1;
//     TIM15->DIER |= TIM_DIER_UIE;
//     NVIC->ISER[0] = 1 << TIM15_IRQn;
//     TIM15->CR1 |= TIM_CR1_CEN;
// }

// int joystick_main(void)
// {
//     setup_adc();
//     setup_tim16();
//     init_tim15();

//     while (1)
//         ;
// }
