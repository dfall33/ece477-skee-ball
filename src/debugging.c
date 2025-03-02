#include "debugging.h"

void micro_wait();

void flash_pb6(int times, int delay)
{
    for (int i = 0; i < times; i++)
    {
        GPIOB->ODR |= GPIO_ODR_6;  // Set PB6 high
        micro_wait(delay);         // Wait for delay
        GPIOB->ODR &= ~GPIO_ODR_6; // Set PB6 low
        micro_wait(delay);         // Wait for delay
    }
}