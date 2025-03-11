#include "ultrasonic.h"
#include "stm32f0xx.h"

void micro_wait(int);

volatile int pulse_timed_out = 0;
volatile int search_timed_out = 0;

void time_out_pulse()
{
    pulse_timed_out = 1;
}

void time_out_hcsr04_search()
{
    search_timed_out = 1;
}

int read_hcsr04(int index)
{
    switch (index)
    {
    case 0:
        return wait_for_echo(GPIO_IDR_1, GPIO_ODR_6);
    case 1:
        return wait_for_echo(GPIO_IDR_2, GPIO_ODR_7);
    case 2:
        return wait_for_echo(GPIO_IDR_3, GPIO_ODR_8);
    case 3:
        return wait_for_echo(GPIO_IDR_4, GPIO_ODR_9);
    case 4:
        return wait_for_echo(GPIO_IDR_5, GPIO_ODR_10);
    default:
        return 0;
    }
}

void pulse_hcsr04_trigger()
{
    // Set PC0 high
    GPIOC->BSRR = GPIO_BSRR_BS_0;
    micro_wait(10);
    // Set PC0 low
    GPIOC->BSRR = GPIO_BSRR_BR_0;
}

void start_hcsr04_pulse_timer()
{
    TIM14->DIER |= TIM_DIER_UIE;
    TIM14->CNT = 0;
    TIM14->CR1 |= TIM_CR1_CEN;
}

void stop_hcsr04_pulse_timer()
{
    TIM14->CR1 &= ~TIM_CR1_CEN;
    TIM14->DIER &= ~TIM_DIER_UIE;
}

void start_hcsr04_search_timer()
{
    TIM14->DIER |= TIM_DIER_UIE;
    TIM14->CNT = 0;
    TIM14->CR1 |= TIM_CR1_CEN;
}

void stop_hcsr04_search_timer()
{
    TIM14->CR1 &= ~TIM_CR1_CEN;
    TIM14->DIER &= ~TIM_DIER_UIE;
}

void send_hcsr04_pulse(uint32_t pin)
{
    // GPIOC->ODR &= ~GPIO_ODR_0;
    // micro_wait(10);
    // GPIOC->ODR |= GPIO_ODR_0;
    // micro_wait(10);
    // GPIOC->ODR &= ~GPIO_ODR_0;

    GPIOC->ODR |= pin;
    micro_wait(10);
    GPIOC->ODR &= ~pin;
}

int wait_for_echo(volatile uint32_t idr_pin, uint32_t odr_pin)
{
    pulse_timed_out = 0;
    start_hcsr04_pulse_timer();
    volatile int start = 0;
    volatile int duration = 0;
    send_hcsr04_pulse(odr_pin);
    int offset = TIM14->CNT;
    while (!pulse_timed_out || start)
    {
        if ((GPIOC->IDR & idr_pin) && start == 0)
        {
            start = TIM14->CNT;
        }

        if (!(GPIOC->IDR & idr_pin) && start != 0)
        {
            duration = TIM14->CNT - start - offset;
            // TIM14->CR1 &= ~TIM_CR1_CEN;
            // TIM14->DIER &= ~TIM_DIER_UIE;
            stop_hcsr04_pulse_timer();
            pulse_timed_out = 0;
            break;
        }
    }
    // stop_hcsr04_timer();
    if (pulse_timed_out)
    {
        return 0;
    }
    else if (duration)
    {
        // return duration <= HCSR04_PULSE_THRESHOLD_US && duration > 0 ? duration : 0;
        return duration > 0 && duration <= 250 ? duration : 0;
    }
    else
    {
        return 0;
    }
}

int search_hcsr04(int stability_count)
{
    search_timed_out = 0;
    start_hcsr04_search_timer();
    volatile int count = 0;
    int duration = 0;
    while (!search_timed_out)
    {
        for (int i = 0; i < 5; i++)
        {
            count++;

            duration = read_hcsr04(i);
            if (duration && count >= stability_count)
            {
                stop_hcsr04_search_timer();
                return i;
            }

            micro_wait(HCSR04_GAP_TIME_US);
        }
    }

    stop_hcsr04_search_timer();
    return BALL_NOT_FOUND;
}