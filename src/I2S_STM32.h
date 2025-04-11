#ifndef __I2S_STM32_H
#define __I2S_STM32_H

#include <stdint.h>

/* ----- Function Prototypes ----- */

/**
 * @brief Initializes the SPI2 peripheral for I2S communication.
 * 
 */
void init_spi2(void);

/**
 * @brief Initializes the I2S peripheral for audio playback.
 * 
 */
void init_i2s(void);

/**
 * @brief Sets up the DMA for audio playback.
 * 
 * @param audio_buf Pointer to the audio buffer.
 * @param buf_size Size of the audio buffer.
 */
void spi2_setup_dma(uint16_t *audio_buf, uint32_t buf_size);

/**
 * @brief Enables the DMA for audio playback.
 * 
 */
void spi2_enable_dma(void);

/**
 * @brief Reads PCM data from the audio buffer.
 * 
 * @param buffer Pointer to the buffer to store the audio data.
 * @param offset Offset in the buffer to start writing data.
 * @param length Number of samples to read.
 */
void read_pcm_data(uint16_t *buffer, uint32_t offset, uint32_t length);

/**
 * @brief Plays the audio file.
 * 
 * @return int 0 on success
 */
int play_sound();
#endif // __I2S_STM32_H