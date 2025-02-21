#ifndef __NHD0440AZ_H__
#define __NHD0440AZ_H__

#include "stm32f0xx.h"
#include "stdint.h"

#define NHD_DATA_HOLD_TIME = 10 // 10 ns

void init_display();

void pulse_rclk(void);
void load_shift_registers(uint16_t data);
void spi_char(char character, uint8_t line_pair);
void spi_write_str(const char *string, uint8_t line);

void spi_cmd_top_two(uint16_t data);
void spi_cmd_bottom_two(uint16_t data);

#endif // __NHD0440AZ_H__