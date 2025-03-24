#include "nhd_0440az.h"

void micro_wait();

void init_display()
{

    /* ----- Setup Top Two Lines ----- */
    micro_wait(2000); // Wait for 2ms

    spi_cmd_top_two(0x0038); // Function set
    micro_wait(2000);        // Wait for 2ms

    spi_cmd_top_two(0x0008); // Display off
    micro_wait(2000);        // Wait for 2ms

    spi_cmd_top_two(0x0001); // Clear display
    micro_wait(2000);        // Wait for 2ms

    spi_cmd_top_two(0x0006); // Entry mode set
    micro_wait(2000);        // Wait for 2ms

    spi_cmd_top_two(0x0002); // Return home
    micro_wait(2000);        // Wait for 2ms

    spi_cmd_top_two(0x000C); // Display on, cursor off
    micro_wait(2000);        // Wait for 2ms

    spi_cmd_top_two(0x01); // Clear display

    /* ----- Setup Bottom Two Lines ----- */
    micro_wait(2000); // Wait for 2ms

    spi_cmd_bottom_two(0x0038); // Function set
    micro_wait(2000);           // Wait for 2ms

    spi_cmd_bottom_two(0x0008); // Display off
    micro_wait(2000);           // Wait for 2ms

    spi_cmd_bottom_two(0x0001); // Clear display
    micro_wait(2000);           // Wait for 2ms

    spi_cmd_bottom_two(0x0006); // Entry mode set
    micro_wait(2000);           // Wait for 2ms

    spi_cmd_bottom_two(0x0002); // Return home
    micro_wait(2000);           // Wait for 2ms

    spi_cmd_bottom_two(0x000C); // Display on, cursor off
    micro_wait(2000);           // Wait for 2ms

    spi_cmd_bottom_two(0x01); // Clear display
    micro_wait(2000);         // Wait for 2ms
}

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

void spi_char(char character, uint8_t line_pair)
{
    GPIOA->ODR &= ~(GPIO_ODR_2 | GPIO_ODR_3);
    micro_wait(1);

    uint16_t data = 0x0200 | character; // Set the RS bit to 1 for data
    load_shift_registers(data);

    if (line_pair == 0)
    {
        // Set the NHD E1 pin high
        GPIOA->ODR |= GPIO_ODR_2;
    }
    else
    {
        // Set the NHD E2 pin high
        GPIOA->ODR |= GPIO_ODR_3;
    }
    // // set e1 high to prepare for loading data
    // GPIOA->ODR |= GPIO_ODR_2;

    // hold enable high for a bit
    micro_wait(1);

    if (line_pair == 0)
    {
        // Set the NHD E1 pin low
        GPIOA->ODR &= ~GPIO_ODR_2;
    }
    else
    {
        // Set the NHD E2 pin low
        GPIOA->ODR &= ~GPIO_ODR_3;
    }

    // wait for a short time (make sure satisfying the hold time of the data after falling edge of e1)
    micro_wait(1);
}

void spi_write_str(const char *string, uint8_t line)
{

    // set the cursor to the correct position
    if (line == 0)
    {
        spi_cmd_top_two(0x02); // Move the cursor to the home position
    }
    else if (line == 1)
    {
        spi_cmd_top_two(0xC0); // Move the cursor to the home position
    }
    else if (line == 2)
    {
        spi_cmd_bottom_two(0x02); // Move the cursor to the home position
    }
    else if (line == 3)
    {
        spi_cmd_bottom_two(0xC0); // Move the cursor to the home position
    }

    // spi_cmd_top_two(0xC0); // Move the cursor to the home position
    int line_pair = line / 2;

    while (*string != '\0')
    {
        spi_char(*string, line_pair);
        string++;
    }
    spi_char(120, line_pair);
}

void spi_cmd_top_two(uint16_t data)
{

    // make sure e1, e2 are low
    GPIOA->ODR &= ~(GPIO_ODR_2 | GPIO_ODR_3);
    // wait for a short time
    micro_wait(1);

    // set e1 high to prepare for loading data
    GPIOA->ODR |= GPIO_ODR_2;

    // hold it high for a bit
    micro_wait(1);

    // prepare the parallel data
    load_shift_registers(data);

    // hold it high for a bit
    micro_wait(1);

    // Set the NHD E1 pin low
    GPIOA->ODR &= ~GPIO_ODR_2;

    // wait for a short time (make sure satisfying the hold time of the data after falling edge of e1)
    micro_wait(1);
}

void spi_cmd_bottom_two(uint16_t data)
{

    // make sure e1, e2 are low
    GPIOA->ODR &= ~(GPIO_ODR_2 | GPIO_ODR_3);
    // wait for a short time
    micro_wait(1);

    // set e2 high to prepare for loading data
    GPIOA->ODR |= GPIO_ODR_3;

    // hold it high for a bit
    micro_wait(1);

    // prepare the parallel data
    load_shift_registers(data);

    // hold it high for a bit
    micro_wait(1);

    // Set the NHD E2 pin low
    GPIOA->ODR &= ~GPIO_ODR_3;

    // wait for a short time (make sure satisfying the hold time of the data after falling edge of e2)
    micro_wait(1);
}
