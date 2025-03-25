#include "button.h"
// #include "game.h"

// extern game_active();

volatile int button_released = 0;
volatile int button_timed_out = 0;
volatile int button_pressed = 0;

extern game_state; // get the game state variable from game.c

void start_button_press()
{

    // if we are here, then we are in the active state and just pressed the button, so transition to the button press state
    game_state = 2; // go to button press state

    start_button_timer();
    button_pressed = 1;
    button_released = 0;
    button_timed_out = 0;

    // game_active();
}

void stop_button_press()
{
    // uint32_t duration_us = TIM3->CNT;

    // if (duration_us > BUTTON_MAX_PRESS_US)
    //     return BUTTON_MAX_PRESS_US;
    // else
    //     return duration_us;

    button_pressed = 0;
    button_released = 1;
    button_timed_out = 0;
    stop_button_timer();
}

void time_out_button()
{
    button_pressed = 0;
    button_released = 0;
    button_timed_out = 1;
    stop_button_timer();
}

void start_button_timer()
{
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CNT = 0;
    TIM3->CR1 |= TIM_CR1_CEN;
}

void stop_button_timer()
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->DIER &= ~TIM_DIER_UIE;
}

void disable_button_interrupt()
{
    // disable the external interrupt associated with the button
    // EXTI->IMR &= ~EXTI_IMR_IM0;
    NVIC_DisableIRQ(EXTI0_1_IRQn);
}

void enable_button_interrupt()
{
    // enable the external interrupt associated with the button
    // EXTI->IMR |= EXTI_IMR_IM0;
    NVIC_EnableIRQ(EXTI0_1_IRQn);
}

int get_press_duration()
{

    // wait for the button to be released, or for the press to time out (max press duration exceeded)
    while (!button_released && !button_timed_out)
        ;

    if (button_released)
    {
        button_released = 0;
        button_pressed = 0;
        button_timed_out = 0;

        // scale the timer counter to an integer between 1 and 10
        // (interpoliation of press duration 0us <= t <= BUTTON_MAX_PRESS_US to digital value 1 <= x <= 10)
        return (10 * TIM3->CNT) / BUTTON_MAX_PRESS_US;
    }
    else
    {
        button_timed_out = 0;
        button_pressed = 0;
        button_released = 0;
        return 10;
    }
}

// enable exti on pc13
void init_button_exti()
{
    // enable clock for syscfg
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // set PC13 as external interrupt source
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    // enable interrupt on rising edge and falling edge
    EXTI->RTSR |= EXTI_RTSR_TR13;
    EXTI->FTSR |= EXTI_FTSR_TR13;

    // enable interrupt on line 13
    EXTI->IMR |= EXTI_IMR_MR13;

    // enable the interrupt in the NVIC
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void init_button_gpio()
{
    // enable clock for GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // set PC13 to input mode
    GPIOC->MODER &= ~(GPIO_MODER_MODER13);
}