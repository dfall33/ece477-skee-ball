#include "stm32f0xx.h"
#include <math.h> // for M_PI
#include <stdint.h>
#include <stdio.h>

void nano_wait(int);
void micro_wait(int);

void spi_cmd(unsigned int);
void spi_clear();
void spi_data(unsigned int);
void spi1_init_oled();
void spi1_display1(const char *);
void spi1_display2(const char *);
void init_spi1();
void init_gpio();
void init_exti();
void setup_tim14();
void read_hcsr04();

/* ----- Global Variables ----- */

#define FALSE 0
#define TRUE 1
volatile int ultrasonic_timed_out = FALSE;
volatile int ultrasonic_detected = FALSE;

void init_gpio()
{
    // enable clock to GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // configure PA0 to be in output mode
    GPIOA->MODER &= ~(GPIO_MODER_MODER0); // reset it
    GPIOA->MODER |= GPIO_MODER_MODER0_0;  // set it to output mode

    // configure PA1 to be in input mode
    GPIOA->MODER &= ~(GPIO_MODER_MODER1); // reset it
}

void init_exti()
{
    // enable clock to SYSCFG
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // configure PA1 to be an EXTI line
    SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI1); // reset it
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; // set it to PA1

    // enable the interrupt on EXTI1
    EXTI->IMR |= EXTI_IMR_IM1;

    // configure the trigger to be on the rising edge
    EXTI->RTSR |= EXTI_RTSR_TR1;

    // enable the interrupt in the NVIC
    NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void EXTI0_1_IRQHandler()
{
    // clear the interrupt flag
    EXTI->PR |= EXTI_PR_PR1;

    // set the ultrasonic_detected flag
    ultrasonic_detected = TRUE;
}

int us_to_cm(int us)
{
    return us / 58;
}

void setup_tim14()
{

    // enable clock to TIM14
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;

    // TIM14->ARR = 1440000;
    TIM14->PSC = 48 - 1; // 48 MHz / 48 = 1 MHz
    TIM14->ARR = 30000;  // 1 MHz / 30000 = 33.33 Hz

    // enable the interrupt on timer overflow
    TIM14->DIER |= TIM_DIER_UIE;

    // enable the interrupt in the NVIC
    NVIC_EnableIRQ(TIM14_IRQn);

    // enable the timer
    // TIM14->CR1 |= TIM_CR1_CEN;

    // don't enable the timer yet, because we want to start it when we start the ultrasonic sensor
}

void TIM14_IRQHandler()
{
    // clear the interrupt flag
    TIM14->SR &= ~TIM_SR_UIF;

    // time out the ultrasonic sensor
    ultrasonic_timed_out = TRUE;
}

void read_hcsr04()
{
    // set PA0 to high
    GPIOA->ODR |= GPIO_ODR_0;

    // hold for 40 microseconds
    micro_wait(40);

    // set PA0 to low
    GPIOA->ODR &= ~GPIO_ODR_0;

    // make sure flags are reset
    ultrasonic_timed_out = FALSE;
    ultrasonic_detected = FALSE;

    spi_clear();
    spi1_display1("reading");

    // enable the timer
    TIM14->CNT = 0;
    TIM14->CR1 |= TIM_CR1_CEN;

    spi1_display2("reading");

    // wait for the ultrasonic sensor to time out
    while (!ultrasonic_timed_out && !ultrasonic_detected)
        ;

    // disable the timer
    spi_clear();
    spi1_display1("done");

    // if the ultrasonic sensor timed out, print "timed out"
    if (ultrasonic_timed_out)
    {
        // printf("timed out\n");
        spi_clear();
        spi1_display1("timed out");
    }

    // if the ultrasonic sensor detected something, print the distance
    if (ultrasonic_detected)
    {
        int distance = us_to_cm(TIM14->CNT);
        spi_clear();

        // create a string to hold the distance
        char distance_str[16];
        snprintf(distance_str, 16, "%d", distance);

        // spi1_display1(distance_str);
        spi1_display2(distance_str);
    }
}

void init_spi1()
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

    // configure for 10-bit word size
    SPI1->CR2 |= SPI_CR2_DS_3 | SPI_CR2_DS_0; // 0x1001 used for 10-bit word size
    SPI1->CR2 &= ~(SPI_CR2_DS_2 | SPI_CR2_DS_1);

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

void spi_cmd(unsigned int data)
{
    // wait until spi1 TX is empty
    while ((SPI1->SR & SPI_SR_TXE) == 0)
        ;

    // copy data to spi1 data register
    SPI1->DR = data;
}
void spi_data(unsigned int data)
{
    // call spi_cmd with (data | 0x200)
    unsigned int val = data | 0x200;
    spi_cmd(val);
}
void spi1_init_oled()
{
    int millisecond = 1000000;
    // wait 1ms with nano_wait
    nano_wait(millisecond);

    // call spi_cmd with 0x38 to do a function set
    spi_cmd(0x38);

    // call spi_cmd with 0x08 to turn off the display
    spi_cmd(0x08);

    // call spi_cmd with 0x01 to clear the display
    spi_cmd(0x01);

    // wait 2ms using nano_wait
    nano_wait(2 * millisecond);

    // call spi_cmd with 0x06 to set the entry mode
    spi_cmd(0x06);

    // call spi_cmd with 0x02 to move the cursor to the home position
    spi_cmd(0x02);

    // call spi_cmd with 0x0c to turn on the display
    spi_cmd(0x0c);
}
void spi1_display1(const char *string)
{
    // move the cursor to the home position
    spi_cmd(0x02);

    // for each character in the string
    // call spi_data with the character

    // iterate over the string using string++
    while (*string != '\0')
    {
        spi_data(*string);
        string++;
    }
}

void spi_clear()
{
    // move the cursor to the home position
    spi_cmd(0x02);

    // for each character in the string
    // call spi_data with the character

    // iterate over the string using string++
    for (int i = 0; i < 16; i++)
    {
        spi_data(' ');
    }

    // clear the second row
    spi_cmd(0xc0);
    for (int i = 0; i < 16; i++)
    {
        spi_data(' ');
    }
}

void spi1_display2(const char *string)
{
    // move the cursor to the second row
    spi_cmd(0xc0);

    // for each character in the string
    // call spi_data with the character

    // iterate until null terminator
    while (*string != '\0')
    {
        spi_data(*string);
        string++;
    }
}

int main(void)
{
    internal_clock();
    // printf("Hello, World!\n");
    init_gpio();
    init_exti();
    setup_tim14();

    init_spi1();
    spi1_init_oled();
    spi1_display1("Hello, World!");

    read_hcsr04();

    while (1)
        ;
}
