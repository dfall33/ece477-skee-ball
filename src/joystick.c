#include "stm32f0xx.h"
#include <stdio.h>
#include "joystick.h"

// Servo Parameters
#define SERVO_MIN_PULSE 500  // 500us
#define SERVO_MAX_PULSE 2500 // 2500us

volatile int degrees;
volatile int adc = 0;

extern void spi_write_str(char *, int);

extern void led_high(int);
extern void led_off();

// void disable_joystick_interrupt(void)
void disable_joystick(void)
{
    TIM2->CR1 &= ~TIM_CR1_CEN; // Stop the timer for reading joystick ADC values
}
// {
//     NVIC_DisableIRQ(EXTI0_1_IRQn);
// }

void enable_joystick(void)
{

    // NVIC_EnableIRQ(EXTI0_1_IRQn);
    TIM2->CR1 |= TIM_CR1_CEN; // Start the timer for reading joystick ADC values
}

void enable_servo(void)
{
    // Enable the servo by setting up the timer and PWM
    TIM16->CR1 |= TIM_CR1_CEN;
}

void disable_servo(void)
{
    // Enable the servo by setting up the timer and PWM
    TIM16->CR1 &= ~TIM_CR1_CEN;
}

void setup_joystick_exti(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    EXTI->IMR |= EXTI_IMR_MR1;
}

// Setup ADC for Joystick Input (PA1)
void setup_adc(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= 0xC;
    RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
    RCC->CR2 |= RCC_CR2_HSI14ON;
    while (!(RCC->CR2 & RCC_CR2_HSI14RDY))
        ;
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY))
        ;
    ADC1->CHSELR |= ADC_CHSELR_CHSEL1;
    while (!(ADC1->ISR & ADC_ISR_ADRDY))
        ;
}

// Setup PWM on TIM16 Channel 1 (PB8)
void setup_tim16(void)
{

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

    GPIOB->MODER &= ~0x00030000;
    GPIOB->MODER |= GPIO_MODER_MODER8_1;
    GPIOB->AFR[1] &= ~0x0000000F;
    GPIOB->AFR[1] |= 0x2;

    TIM16->PSC = 16 - 1;
    // TIM16->PSC = 96 - 1;

    TIM16->ARR = 60000 - 1;
    TIM16->BDTR |= TIM_BDTR_MOE;
    // PWM Mode 1 on CH1
    TIM16->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM16->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos); // PWM mode 1
    TIM16->CCMR1 |= TIM_CCMR1_OC1PE;

    // Set duty cycle
    // TIM16->CCR1 = 1500;

    TIM16->CCER |= TIM_CCER_CC1E;
    TIM16->EGR |= TIM_EGR_UG;
    // TIM16->CR1 |= TIM_CR1_CEN;
    // move_to_angle(degrees);
}

// Convert ADC Value to Degree
int map_adc_to_degrees(int adc_val)
{
    int adc_min = 0, adc_max = 4095;
    int deg_min = -2, deg_max = 2;
    int val = ((adc_val - adc_min) * (deg_max - deg_min) / (adc_max - adc_min)) + deg_min;
    if (adc_val <= 4000 && adc_val >= 1800)
        val = 0;
    return val;
}

// Move Servo to Specified Angle
void move_to_angle(int angle)
{

    int pulse_width = SERVO_MIN_PULSE + ((angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE)) / 180);
    TIM16->CCR1 = 3 * pulse_width; // Set duty cycle
    printf("Moving to %d degrees (Pulse: %d)\n", angle, pulse_width);
}

// Read Joystick ADC Value
int read_joystick(void)
{
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC))
        ;
    return ADC1->DR;
}

// Timer 2 ISR
void TIM2_IRQHandler()
{
    TIM2->SR &= ~TIM_SR_UIF;
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC))
        ;

    int adc_val = ADC1->DR;
    adc = adc_val;

    int inc_degrees = map_adc_to_degrees(adc_val);
    if ((degrees + inc_degrees) >= 0 && (degrees + inc_degrees) <= 180)
    {
        degrees += inc_degrees;
    }

    move_to_angle(degrees);
}

// Setup TIM15 for Periodic Joystick Reading
void init_tim2(void)
{

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 480 - 1;
    // TIM2->PSC = 2880 - 1;
    // TIM2->ARR = 1000 - 1;
    TIM2->ARR = 5000 - 1; 
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] = 1 << TIM2_IRQn;
    NVIC_EnableIRQ(TIM2_IRQn); // Enable the interrupt for TIM2
    // TIM2->CR1 |= TIM_CR1_CEN;
}
