#include "debug.h"

void setup_debug_ports()
{

    // LEDs are on PB3-PB7, so enable clock to GPIOB
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Set PB3-PB7 to output mode
    GPIOB->MODER &= ~(
        GPIO_MODER_MODER3 |
        GPIO_MODER_MODER4 |
        GPIO_MODER_MODER5 |
        GPIO_MODER_MODER6 |
        GPIO_MODER_MODER7);

    GPIOB->MODER |= (GPIO_MODER_MODER3_0 |
                     GPIO_MODER_MODER4_0 |
                     GPIO_MODER_MODER5_0 |
                     GPIO_MODER_MODER6_0 |
                     GPIO_MODER_MODER7_0);
}

void led_high(int8_t index)
{
    if (index < 0 || index > 4)
        return;

    switch (index)
    {

    // for GPIOB3
    case 0:
        GPIOB->BSRR |= GPIO_BSRR_BS_3;
        break;

    // for GPIOB4
    case 1:
        GPIOB->BSRR |= GPIO_BSRR_BS_4;
        break;

    // for GPIOB5
    case 2:
        GPIOB->BSRR |= GPIO_BSRR_BS_5;
        break;

    // for GPIOB6
    case 3:
        GPIOB->BSRR |= GPIO_BSRR_BS_6;
        break;

    // for GPIOB7
    case 4:
        GPIOB->BSRR |= GPIO_BSRR_BS_7;
        break;
    }
}