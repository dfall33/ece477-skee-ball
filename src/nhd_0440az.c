#include "nhd_0440az.h"

void micro_wait();

void pulse_rclk()
{
    // make sure transmission is complete
    while ((SPI1->SR & SPI_SR_BSY) != 0)
        ;

    // // wait til the SPI buffer is empty
    while ((SPI1->SR & SPI_SR_TXE) == 0)
        ;

    // Set the RCLK pin high
    GPIOA->ODR |= GPIO_ODR_4;
    // wait for a short time
    micro_wait(1);

    // Set the RCLK pin low
    GPIOA->ODR &= ~GPIO_ODR_4;
    // wait for a short time
    micro_wait(1);
}

void load_shift_registers(uint16_t data)
{
    while ((SPI1->SR & SPI_SR_TXE) == 0)
        ;
    SPI1->DR = data; // Load the data into the SPI data register
    pulse_rclk();    // Pulse the RCLK pin to latch the data
}