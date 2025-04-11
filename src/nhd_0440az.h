#ifndef __NHD0440AZ_H__
#define __NHD0440AZ_H__

#include "stm32f0xx.h"
#include "stdint.h"

/* ----- Constants ----- */
#define CHARS_PER_LINE 40 // number of characters per line on the display 

/* ----- Function Prototypes ----- */

/**
 * @brief Flash initialization sequence for the display
 * 
 */
void init_display();

/**
 * @brief Pulse GPIO pin to prepare the shift register outputs 
 * 
 */
void pulse_rclk(void);

/**
 * @brief Load the shift registers with the given data
 * 
 * @param data The data to load into the shift registers
 */
void load_shift_registers(uint16_t data);

/**
 * @brief Send a character to the display
 * 
 * @param character The character to send
 * @param line_pair The line pair to send the character to (0 for top two lines, 1 for bottom two lines)
 */
void spi_char(char character, uint8_t line_pair);

/**
 * @brief Write a string to the display
 * 
 * @param string The string to write
 * @param line The line to write the string to (0-3)
 */
void spi_write_str(const char *string, uint8_t line);

/**
 * @brief Send a command to the top two lines of the display
 * 
 * @param data The command to send
 */
void spi_cmd_top_two(uint16_t data);

/**
 * @brief Send a command to the bottom two lines of the display
 * 
 * @param data The command to send
 */
void spi_cmd_bottom_two(uint16_t data);

/**
 * @brief Set up SPI for the display 
 * 
 */
void setup_spi_display(); 

/**
 * @brief Set up GPIO for the display 
 * 
 */
void setup_gpio_display(); 

/**
 * @brief Set up all interfaces, ports necessary 
 * 
 */
void setup_display(); 

/**
 * @brief Test the display (write unique string on each line )
 * 
 */
void test_display(); 

/**
 * @brief Clear the display 
 * 
 */
void clear_display(); 

/**
 * @brief Display a progress bar on the display 
 * 
 * @param amount The amount of progress (0-10)
 * @param line The line to display the progress bar on (0-3)
 */
void progress_bar(uint8_t amount, uint8_t line); 

#endif // __NHD0440AZ_H__