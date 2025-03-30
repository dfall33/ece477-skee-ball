#include "ultrasonic.h"
#include "stm32f0xx.h"

void micro_wait(int);

volatile int pulse_timed_out = 0;
volatile int search_timed_out = 0;

extern void led_high(int index); 
extern void led_low(int index);

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
    // switch (index)
    // {
    // case 0:
    //     return wait_for_echo(GPIO_IDR_1, GPIO_ODR_6);
    // case 1:
    //     return wait_for_echo(GPIO_IDR_2, GPIO_ODR_7);
    // case 2:
    //     return wait_for_echo(GPIO_IDR_3, GPIO_ODR_8);
    // case 3:
    //     return wait_for_echo(GPIO_IDR_4, GPIO_ODR_9);
    // case 4:
    //     return wait_for_echo(GPIO_IDR_5, GPIO_ODR_10);
    // default:
    //     return 0;
    // }

    switch (index)
    {
    // first sensor echo=PC6, trigger=PC7
    case 0:
        return wait_for_echo(
            GPIOC,      // port,
            GPIO_IDR_6, // idr pin
            GPIO_ODR_7  // odr pin
        );

    // second sensor echo=PC8, trigger=PC9
    case 1:
        return wait_for_echo(
            GPIOC,      // port,
            GPIO_IDR_8, // idr pin
            GPIO_ODR_9  // odr pin
        );

    // third sensor echo=PA8, trigger=PA9
    case 2:
        return wait_for_echo(
            GPIOA,      // port,
            GPIO_IDR_8, // idr pin
            GPIO_ODR_9  // odr pin
        );

    // third sensor echo=PA10, trigger=PA11
    case 3:
        return wait_for_echo(
            GPIOA,       // port,
            GPIO_IDR_10, // idr pin
            GPIO_ODR_11  // odr pin
        );

    // fifth sensor echo = PC14, trigger = PC15
    case 4:
        return wait_for_echo(
            GPIOC,       // port,
            GPIO_IDR_14, // idr pin
            GPIO_ODR_15  // odr pin
        );
    }
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

void send_hcsr04_pulse(GPIO_TypeDef *port, uint32_t pin)
{

    // GPIOC->ODR |= pin;
    port->ODR |= pin;
    micro_wait(10);
    // GPIOC->ODR &= ~pin;
    port->ODR &= ~pin;
}

int wait_for_echo(GPIO_TypeDef *port, volatile uint32_t idr_pin, uint32_t odr_pin)
{
    pulse_timed_out = 0;
    start_hcsr04_pulse_timer();
    volatile int start = 0;
    volatile int duration = 0;
    send_hcsr04_pulse(port, odr_pin);
    int offset = TIM14->CNT;
    while (!pulse_timed_out || start)
    {
        led_high(4); 
        // if ((GPIOC->IDR & idr_pin) && start == 0)
        if ((port->IDR & idr_pin) && start == 0)
        {
            start = TIM14->CNT;
            // led_high(1); 
        }

        // if (!(GPIOC->IDR & idr_pin) && start != 0)
        if (!(port->IDR & idr_pin) && start != 0)
        {
            duration = TIM14->CNT - start - offset;
            stop_hcsr04_pulse_timer();
            pulse_timed_out = 0;
            // led_high(3); 
            break;
        }
    }

    led_low(4); 
    stop_hcsr04_pulse_timer();
    if (pulse_timed_out)
    {
        led_high(1); 
        return 0;
    }
    else if (duration)
    {

        // return duration <= HCSR04_PULSE_THRESHOLD_US && duration > 0 ? duration : 0;
        led_high(2); 
        return duration > 0 && duration <= 250 ? duration : 0;
    }
    else
    {
        led_high(3); 
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


int test_sensor(int8_t index)
{
    
    // This function is used to test a single ultrasonic sensor by its index
    // It will return the duration of the echo received from the sensor
    // If no echo is received, it will return 0

    int duration = read_hcsr04(index);
    if (duration > 0)
    {
        // Successfully received an echo from the sensor
        return duration;
    }
    else
    {
        // No echo received from the sensor
        return 0;
    }

}


void setup_ultrasonic_ports()
{
    
    // This function is used to setup the ultrasonic sensor ports for input and output

    // Enable clock for GPIOC and GPIOA (assuming sensors are connected to these ports)
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN;

    // reset all (puts into input mode)
    GPIOC->MODER &= ~(
        GPIO_MODER_MODER6 |
        GPIO_MODER_MODER7 |
        GPIO_MODER_MODER8 |
        GPIO_MODER_MODER9 | 
        GPIO_MODER_MODER14 | 
        GPIO_MODER_MODER15
    );

    GPIOA->MODER &= ~(
        GPIO_MODER_MODER8 |
        GPIO_MODER_MODER9 |
        GPIO_MODER_MODER10 |
        GPIO_MODER_MODER11);

    // configure trigger pins for echo mode

    GPIOC->MODER |= (GPIO_MODER_MODER7_0 |
                     GPIO_MODER_MODER9_0 |
                     GPIO_MODER_MODER15_0);

    GPIOA->MODER |= (GPIO_MODER_MODER9_0 | 
                     GPIO_MODER_MODER11_0); // PA9 and PA11 are used for the third and fourth sensors respectively
}