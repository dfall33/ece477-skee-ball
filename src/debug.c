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

void led_low(int8_t index)
{
    if (index < 0 || index > 4)
        return;

    switch (index)
    {
        case 0:
            GPIOB->BSRR |= GPIO_BSRR_BR_3; // Set PB3 low
            break;
        case 1:
            GPIOB->BSRR |= GPIO_BSRR_BR_4; // Set PB4 low
            break;
        case 2:
            GPIOB->BSRR |= GPIO_BSRR_BR_5; // Set PB5 low
            break;
        case 3:
            GPIOB->BSRR |= GPIO_BSRR_BR_6; // Set PB6 low
            break;
        case 4:
            GPIOB->BSRR |= GPIO_BSRR_BR_7; // Set PB7 low
            break;
    }
}

void led_off()
{
    
    // Turn off all LEDs by setting all bits low
    GPIOB->BSRR |= (GPIO_BSRR_BR_3 |  // Set PB3 low
                    GPIO_BSRR_BR_4 |  // Set PB4 low
                    GPIO_BSRR_BR_5 |  // Set PB5 low
                    GPIO_BSRR_BR_6 |  // Set PB6 low
                    GPIO_BSRR_BR_7);  // Set PB7 low
    // Alternatively, you could loop through each LED and call led_low(index) for each one.
}