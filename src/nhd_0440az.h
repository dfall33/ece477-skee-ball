#ifndef __NHD0440AZ_H__
#define __NHD0440AZ_H__

#include "stm32f0xx.h"
#include "stdint.h"

#define CHARS_PER_LINE 40 // number of characters per line on the display 

void init_display();

void pulse_rclk(void);
void load_shift_registers(uint16_t data);
void spi_char(char character, uint8_t line_pair);
void spi_write_str(const char *string, uint8_t line);

void spi_cmd_top_two(uint16_t data);
void spi_cmd_bottom_two(uint16_t data);

void setup_spi_display(); 
void setup_gpio_display(); 
void setup_display(); 

void test_display(); 

void clear_display(); 
void progress_bar(uint8_t amount, uint8_t line); 

#endif // __NHD0440AZ_H__