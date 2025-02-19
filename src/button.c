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

int get_press_duration()
{

    // if (!button_pressed)
    // {
    //     return 0;
    // }

    while (!button_released && !button_timed_out)
        ;

    if (button_released)
    {
        button_released = 0;
        button_pressed = 0;
        button_timed_out = 0;

        // return 5;
        return (10 * TIM3->CNT) / BUTTON_MAX_PRESS_US;
    }
    else
    {
        button_timed_out = 0;
        button_pressed = 0;
        button_released = 0;
        return -1;
    }
}