#include "stm32f0xx.h"
#include <stdint.h>
#include "audio_data.h"
#include "I2S_STM32.h"

extern const unsigned char wav[];
extern const unsigned int wav_len;
#define WAV_HEADER_SIZE 44

#define AUDIO_BUF_SIZE 256
static uint16_t audio_buf[AUDIO_BUF_SIZE];
static volatile uint32_t audio_pos = WAV_HEADER_SIZE;
static volatile uint8_t dma_half_complete = 0;
static volatile uint8_t dma_full_complete = 0;
static volatile uint8_t audio_done = 0;
//===========================================================================
// Initialize the SPI2 peripheral.
//===========================================================================
void init_spi2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    // Set the mode of PB12, PB13, PB15 to alternate function mode for each of the Timer 3 channels.
    GPIOB->MODER &= ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER13 | GPIO_MODER_MODER15);
    GPIOB->MODER |= (GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1);
    // Some pins can have more than one alternate function associated with them; specifying which alternate function is to be used is the purpose of the GPIOx_AFRL (called AFR[0]) and GPIOx_AFRH (called AFR[1]) registers.
    // PB12 (I2S_ws)
    GPIOB->AFR[1] &= ~(0xf << (4 * (12 - 8))); // clear bits
    GPIOB->AFR[1] |= 0x0 << (4 * (12 - 8));    // set bits to 0 for AF0
    // PB13 (I2S_ck)
    GPIOB->AFR[1] &= ~(0xf << (4 * (13 - 8))); // clear bits
    GPIOB->AFR[1] |= 0x0 << (4 * (13 - 8));    // set bits to 0 for AF0
    // PB15(I2S_sd)
    GPIOB->AFR[1] &= ~(0xf << (4 * (15 - 8))); // clear bits
    GPIOB->AFR[1] |= 0x0 << (4 * (15 - 8));    // set bits to 0 for AF0
    // Ensure that the CR1_SPE bit is clear first.
    SPI2->CR1 &= ~SPI_CR1_SPE;
    // Set the baud rate as low as possible (maximum divisor for BR).
    SPI2->CR1 |= (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0);
    // Configure the interface for a 16-bit word size.
    SPI2->CR2 |= (SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3);
    // Configure the SPI channel to be in "master configuration".
    SPI2->CR1 |= SPI_CR1_MSTR;
    // Set the SS Output enable bit and enable NSSP.
    SPI2->CR2 |= SPI_CR2_SSOE;
    SPI2->CR2 |= SPI_CR2_NSSP;
    // Set the TXDMAEN bit to enable DMA transfers on transmit buffer empty
    SPI2->CR2 |= SPI_CR2_TXDMAEN;
    // Enable the SPI channel.
    SPI2->CR1 |= SPI_CR1_SPE;
}

void spi2_setup_dma(uint16_t *audio_buf, uint32_t buf_size)
{
    //============================================================================
    // setup_dma() + enable_dma()
    //===========================================================================
    //   Enables the RCC clock to the DMA controller
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    // Turn off the enable bit for the channel first
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;
    // Set CMAR to the address of the msg array.
    DMA1_Channel5->CMAR = (uint32_t)audio_buf;
    // Set CPAR to the address of the GPIOB_ODR register.
    DMA1_Channel5->CPAR = (uint32_t)&SPI2->DR;
    // Set CNDTR to 8.
    DMA1_Channel5->CNDTR = buf_size;
    // Set the DIRection for copying from-memory-to-peripheral.
    DMA1_Channel5->CCR |= DMA_CCR_DIR;
    // Set the MINC to increment the CMAR for every transfer.
    DMA1_Channel5->CCR |= DMA_CCR_MINC;
    // Set the Memory datum SIZE to 16-bit.
    DMA1_Channel5->CCR |= DMA_CCR_MSIZE_0;
    // Set the Peripheral datum SIZE to 16-bit.
    DMA1_Channel5->CCR |= DMA_CCR_PSIZE_0;
    // Set the channel for CIRCular operation.
    DMA1_Channel5->CCR |= DMA_CCR_CIRC;

    DMA1_Channel5->CCR |= DMA_CCR_HTIE;   // Half transfer interrupt
    DMA1_Channel5->CCR |= DMA_CCR_TCIE;   // Transfer complete interrupt
    NVIC_EnableIRQ(DMA1_Channel4_5_IRQn); // Enable DMA interrupt
    SPI2->CR2 |= SPI_CR2_TXDMAEN;
}

void DMA1_Channel4_5_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_HTIF5) // Half Transfer Interrupt
    {
        DMA1->IFCR |= DMA_IFCR_CHTIF5; // Clear flag
        dma_half_complete = 1;         // Flag to signal that half buffer needs refilling
    }

    if (DMA1->ISR & DMA_ISR_TCIF5) // Transfer Complete Interrupt
    {
        DMA1->IFCR |= DMA_IFCR_CTCIF5; // Clear flag
        dma_full_complete = 1;         // Flag to signal that full buffer needs refilling
    }
}
void spi2_enable_dma(void)
{
    // Enable the channel.
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

void init_i2s(void)
{

    // disable i2s first
    SPI2->I2SCFGR &= ~(SPI_I2SCFGR_I2SE);

    // Clear I2SMOD bit first

    SPI2->I2SCFGR &= ~SPI_I2SCFGR_I2SMOD;
    // Enable I2S mode
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SMOD;
    // Set I2S in Master mode
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_1;
    // Set I2S standard
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_I2SSTD;
    // Set data length to 16-bit
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_CHLEN;
    // Set clock polarity to low
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_CKPOL;
    SPI2->I2SPR = 110;
    //  Enable I2S
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE;
}

void read_pcm_data(uint16_t *buffer, uint32_t offset, uint32_t length)
{
    uint32_t samples_read = 0;

    while (samples_read < length)
    {
        if (audio_pos >= wav_len) // End of file reached
        {
            audio_done = 1; // Set flag to stop playback
            break;          // Exit loop without overwriting buffer
        }

        buffer[offset + samples_read] = (((uint16_t)wav[audio_pos] - 128) << 8);
        audio_pos++;
        samples_read++;
    }
}
int play_sound(void)
{
    // initialize spi2
    init_spi2();
    // Initialize I2S
    init_i2s();
    // Preload initial audio buffer
    read_pcm_data(audio_buf, 0, AUDIO_BUF_SIZE);

    spi2_setup_dma(audio_buf, AUDIO_BUF_SIZE);
    spi2_enable_dma();
    while (!audio_done) // Loop until full audio file is played
    {
        if (dma_half_complete)
        {
            read_pcm_data(audio_buf, 0, AUDIO_BUF_SIZE / 2); // Refill first half
            dma_half_complete = 0;
        }

        if (dma_full_complete)
        {
            read_pcm_data(audio_buf, AUDIO_BUF_SIZE / 2, AUDIO_BUF_SIZE / 2); // Refill second half
            dma_full_complete = 0;
        }
    }

    // Stop DMA when done
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;

    // reset state 
    audio_pos = WAV_HEADER_SIZE; // Reset position to start of audio data
    audio_done = 0;              // Reset done flag for next playback

    return 0; // Exit after full playback
}
