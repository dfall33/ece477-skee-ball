#include "stm32f0xx.h"
#include <stdio.h>
#include "joystick.h"

// Servo Parameters
#define SERVO_MIN_PULSE  500   // 500us
#define SERVO_MAX_PULSE  2500  // 2500us

volatile int degrees = 45;
volatile int adc = 0;

void disable_joystick_interrupt(void)
{
    NVIC_DisableIRQ(EXTI0_1_IRQn);
}

void enable_joystick_interrupt(void)
{
    NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void setup_joystick_exti(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    EXTI->IMR |= EXTI_IMR_MR1; 
}

void EXTI0_1_IRQHandler(void)
{
    EXTI->PR |= EXTI_PR_PR1;
}

//Setup ADC for Joystick Input (PA1)
void setup_adc(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  
    GPIOA->MODER |= 0xC;                
    RCC->APB2ENR |= RCC_APB2ENR_ADCEN;  
    RCC->CR2 |= RCC_CR2_HSI14ON;        
    while (!(RCC->CR2 & RCC_CR2_HSI14RDY));
    ADC1->CR |= ADC_CR_ADEN;            
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
    ADC1->CHSELR |= ADC_CHSELR_CHSEL1;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

//Setup PWM on TIM16 Channel 1 (PB8)
void setup_tim16(void) {   

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;    
    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;  

    GPIOB->MODER &= ~0x00030000;  
    GPIOB->MODER |= GPIO_MODER_MODER8_1;  
    GPIOB->AFR[1] &= ~0x0000000F; 
    GPIOB->AFR[1] |= 0x2;         

    TIM16->PSC = 16 - 1;                 
    TIM16->ARR = 60000 - 1;         
    TIM16->BDTR |= TIM_BDTR_MOE;
    // PWM Mode 1 on CH1
    TIM16->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM16->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);  // PWM mode 1
    TIM16->CCMR1 |= TIM_CCMR1_OC1PE;         

    // Set duty cycle
    // TIM16->CCR1 = 1500;

    TIM16->CCER |= TIM_CCER_CC1E;
    TIM16->EGR |= TIM_EGR_UG;
    TIM16->CR1 |= TIM_CR1_CEN;
    move_to_angle(degrees);    
}


//Convert ADC Value to Degree
int map_adc_to_degrees(int adc_val) {
    int adc_min = 0, adc_max = 4095;
    int deg_min = -4, deg_max = 4;
    int val = ((adc_val - adc_min) * (deg_max - deg_min) / (adc_max - adc_min)) + deg_min;
    if (adc_val <= 2100 && adc_val >= 1900) val = 0;
    return val;
}

//Move Servo to Specified Angle
void move_to_angle(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    int pulse_width = SERVO_MIN_PULSE + ((angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE)) / 180);
    TIM16->CCR1 = pulse_width;  // Set duty cycle
    printf("Moving to %d degrees (Pulse: %d)\n", angle, pulse_width);
}

//Read Joystick ADC Value
int read_joystick(void) {
    ADC1->CR |= ADC_CR_ADSTART;          
    while (!(ADC1->ISR & ADC_ISR_EOC));  
    return ADC1->DR;                     
}

//Timer 2 ISR
void TIM2_IRQHandler() {
    TIM2->SR &= ~TIM_SR_UIF;
    ADC1->CR |= ADC_CR_ADSTART;
    while(!(ADC1->ISR & ADC_ISR_EOC));
    
    int adc_val = ADC1->DR;
    adc = adc_val;
    
    if(adc_val > 2000 | adc_val < 1900) //SW TRIGGER
    {
        EXTI->SWIER |= EXTI_SWIER_TR1;
    }

    int inc_degrees = map_adc_to_degrees(adc_val);
    if ((degrees + inc_degrees) >= 0 && (degrees + inc_degrees) <= 180) {
        degrees += inc_degrees;
    }

    move_to_angle(degrees);
}

//Setup TIM15 for Periodic Joystick Reading
void init_tim2(void) {

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 480 -1;
    TIM2->ARR = 1000 -1;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] = 1<< TIM2_IRQn;
    TIM2->CR1 |= TIM_CR1_CEN;
}

int main(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  GPIOC->MODER &= ~0x000000ff; //inputs
  GPIOC->PUPDR |=  0x000000aa;
  GPIOC->MODER |=  0x00005500; //outputs
    setup_adc();
    setup_tim16();
    init_tim2();
    GPIOC->ODR = 1 << (8-1);
    while (1);
}
