#include "nhd_0440az.h"

void micro_wait();

void init_display()
{

    // // enable the clock to GPIOA
    // RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // // set GPIOA2 and GPIOA3 as output for NHD E1 and NHD E2 respectively
    // GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3); // Clear the mode bits for PA2 and PA3
    // GPIOA->MODER |= GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0; // Set PA2 and PA3 to output mode

    // // set GPIOA4 as output for RCLK (latch clock)
    // GPIOA->MODER &= ~(GPIO_MODER_MODER4); // Clear the mode bits for PA4
    // GPIOA->MODER |= GPIO_MODER_MODER4_0; // Set PA4 to output mode

    // setup_spi_display(); 

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


void setup_spi_display()
{
    // enable clock for SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~(GPIO_MODER_MODER15 | GPIO_MODER_MODER5 | GPIO_MODER_MODER7);
    GPIOA->MODER |= GPIO_MODER_MODER15_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1;

    // set the alternate function for the pins (SCK, MOSI, NSS are all alternate function 0)

    // reset the alternate function bits for the pins
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL7 | GPIO_AFRL_AFRL5);
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFRH7);

    // configure NSS, SCK, MOSI signals of SPI1 to pins PA15, PA5, PA7, respectively
    // set the alternate function bits for the pins
    GPIOA->AFR[0] |= (0x0 << GPIO_AFRL_AFRL7_Pos) | (0x0 << GPIO_AFRL_AFRL5_Pos);
    GPIOA->AFR[1] |= (0x0 << GPIO_AFRH_AFRH7_Pos);

    // disable SPI1
    SPI1->CR1 &= ~SPI_CR1_SPE;

    // set baud rate to max possible
    SPI1->CR1 |= SPI_CR1_BR;

    // configure for 16-bit word size
    SPI1->CR2 |= SPI_CR2_DS;
    // SPI1->CR2 &= ~(SPI_CR2_DS_2 | SPI_CR2_DS_1);

    // configure the SPI channel to be in master configuration
    SPI1->CR1 |= SPI_CR1_MSTR;

    // set the SS ouptut enable pit and enable NSSP
    SPI1->CR2 |= SPI_CR2_SSOE;
    SPI1->CR2 |= SPI_CR2_NSSP;

    // set the TXDMAEN to enable DMA transfers on transmit buffer empty
    SPI1->CR2 |= SPI_CR2_TXDMAEN;

    // enable the SPI channel
    SPI1->CR1 |= SPI_CR1_SPE;
}

void test_display()
{
    spi_write_str("Line 1", 0); // Write to the first line of the display (top line)
    spi_write_str("Line 2", 1); // Write to the second line of the display (top line)
    spi_write_str("Line 3", 2); // Write to the third line of the display (bottom line)
    spi_write_str("Line 4", 3); // Write to the fourth line of the display (bottom line)
    
}


void setup_gpio_display()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable clock for GPIOA

    // Set GPIOA2 and GPIOA3 as output for NHD E1 and NHD E2 respectively
    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3); // Clear the mode bits for PA2 and PA3
    GPIOA->MODER |= GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0; // Set PA2 and PA3 to output mode

    // Set GPIOA4 as output for RCLK (latch clock)
    GPIOA->MODER &= ~(GPIO_MODER_MODER4); // Clear the mode bits for PA4
    GPIOA->MODER |= GPIO_MODER_MODER4_0; // Set PA4 to output mode
}

void setup_display()
{
    // This function initializes the display by setting up the GPIO pins and sending the initial commands to the display

    setup_gpio_display(); // Setup GPIO pins for the display
    setup_spi_display();  // Setup SPI for the display
    init_display(); // Initialize the display with the necessary commands
}