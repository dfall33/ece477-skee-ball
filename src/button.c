#include "button.h"

volatile int button_released = 0;
volatile int button_timed_out = 0;
volatile int button_pressed = 0;

void start_button_press()
{
    start_button_timer();
    button_pressed = 1;
    button_released = 0;
    button_timed_out = 0;
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
