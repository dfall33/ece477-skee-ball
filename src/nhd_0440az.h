#ifndef __NHD0440AZ_H__
#define __NHD0440AZ_H__

#include "stm32f0xx.h"
#include "stdint.h"

void pulse_rclk(void);
void load_shift_registers(uint16_t data);

#endif // __NHD0440AZ_H__