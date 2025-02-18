#ifndef __I2S_STM32_H
#define __I2S_STM32_H

#include <stdint.h>

void init_spi2(void);
void init_i2s(void);
void spi2_setup_dma(uint16_t *audio_buf, uint32_t buf_size);
void spi2_enable_dma(void);
void read_pcm_data(uint16_t *buffer, uint32_t offset, uint32_t length);
int main_I2S();
#endif // __I2S_STM32_H