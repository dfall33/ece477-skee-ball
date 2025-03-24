#include "stm32f0xx.h"
#include <math.h> // for M_PI
#include <stdint.h>
#include <stdio.h>
#include "ultrasonic.h"
#include "button.h"
#include "nhd_0440az.h"
#include "motor_control.h"
// #include "game.h"
extern game();
extern state;
// extern STATES;

#define PULSE_TIMEOUT_US 30000
#define PULSE_THRESHOLD_US 200

volatile int detected_streak = 0;

/* ----- Function Declarations ----- */
void nano_wait(int);
void micro_wait(int);

void init_gpio();

void init_all();

void init_spi1();

void setup_tim3();
void setup_tim14();
void setup_tim15();

void init_gpio()
{

    // =======================================================
    /* ----- Configure Port A ----- */
    // =======================================================
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    /* ----- Configure PA4 as Output for RCLK ----- */
    GPIOA->MODER &= ~(GPIO_MODER_MODER4);
    GPIOA->MODER |= GPIO_MODER_MODER4_0; // Output mode

    /* ----- Configure PA2 and PA3 as NHD E1 and NHD E2, respectively ----- */
    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
    GPIOA->MODER |= GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0; // Output mode

    // =======================================================
    /* ----- Configure Port C ----- */
    // =======================================================

    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    /* ----- Configure PC0 for Push Button */
    GPIOC->MODER &= ~(GPIO_MODER_MODER0);

    /* ----- Configure PC1-PC10 for ultrasonic sensors -----  */
    // Configure PC1 as input with internal pull-down
    GPIOC->MODER &= ~(GPIO_MODER_MODER1);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR1);

    // Configure PC2 as input with internal pull-down
    GPIOC->MODER &= ~(GPIO_MODER_MODER2);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR2);

    // Configure PC3 as input with internal pull-down
    GPIOC->MODER &= ~(GPIO_MODER_MODER3);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR3);

    // Configure PC4 as input with internal pull-down
    GPIOC->MODER &= ~(GPIO_MODER_MODER4);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR4);

    // Configure PC5 as input with internal pull-down
    GPIOC->MODER &= ~(GPIO_MODER_MODER5);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR5);

    // Configure PC6 as output
    GPIOC->MODER &= ~(GPIO_MODER_MODER6);
    GPIOC->MODER |= GPIO_MODER_MODER6_0; // Output mode

    // Configure PC7 as output
    GPIOC->MODER &= ~(GPIO_MODER_MODER7);
    GPIOC->MODER |= GPIO_MODER_MODER7_0; // Output mode

    // Configure PC8 as output
    GPIOC->MODER &= ~(GPIO_MODER_MODER8);
    GPIOC->MODER |= GPIO_MODER_MODER8_0; // Output mode

    // Configure PC9 as output
    GPIOC->MODER &= ~(GPIO_MODER_MODER9);
    GPIOC->MODER |= GPIO_MODER_MODER9_0; // Output mode

    // Configure PC10 as output
    GPIOC->MODER &= ~(GPIO_MODER_MODER10);
    GPIOC->MODER |= GPIO_MODER_MODER10_0; // Output mode
}

/**
 * @brief Initializes external interrupt to be triggered from button press or release (for ball launching with DC motor)
 *
 */
void init_exti()
{
    // enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // set PC0 as external interrupt source
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC;

    // enable interrupt on rising edge and falling edge
    EXTI->RTSR |= EXTI_RTSR_TR0;
    EXTI->FTSR |= EXTI_FTSR_TR0;

    // enable interrupt on line 0
    EXTI->IMR |= EXTI_IMR_MR0;

    // don't enable the interrupt until the game has started / button can be pressed
    NVIC_EnableIRQ(EXTI0_1_IRQn);
}

/* ----- External Interrupt Handler for PC ----- */

/**
 * @brief External interrupt handler for PC0 (push button)
 *
 */
void EXTI0_1_IRQHandler(void)
{

    EXTI->PR |= EXTI_PR_PR0; // acknowledge the interrupt

    // if the input data register is high, then the button is pressed and being held down,
    // so start the button press timer (will time out after BUTTON_MAX_PRESS_US, defined in src/button.h)
    if (GPIOC->IDR & GPIO_IDR_0)
    {
        start_button_press();
    }

    // if the input data register is low, then the button has been released
    // so stop the button press timer and do whatever is necessary
    else
    {
        stop_button_press();
    }
}

// external interrupt handler for push button (pc13)
void EXTI4_15_IRQHandler(void)
{
    // acknowledge the interrupt
    EXTI->PR |= EXTI_PR_PR13;

    // if the input data register is high, then the button is pressed and being held down,
    // so start the button press timer (will time out after BUTTON_MAX_PRESS_US, defined in src/button.h)
    if (GPIOC->IDR & GPIO_IDR_13)
    {
        start_button_press();
    }

    // if the input data register is low, then the button has been released
    // so stop the button press timer and do whatever is necessary
    else
    {
        stop_button_press();
    }
}

/**
 * @brief This timer is used for timing out individual ultrasonic sensor readings
 *
 */
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

/* ----- This timer is used for timing out the ultrasonic search overall, i.e., search for the ball for N seconds then give up ----- */
void setup_tim15()
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 48 - 1;                   // 48 MHz / 48 = 1 MHz
    TIM15->ARR = HCSR04_SEARCH_TIMEOUT_US; // 1 MHz / 10000 = 100 Hz

    TIM15->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM15_IRQn);
}

/**
 * @brief TIM3 is used for timing how long the button is pressed. The prescaler is set such that
 * the timer counter represents the time pressed in microseconds. The timer has an interrupt so that the button can be constrained to a
 * maximum press time (defined in src/button.h)
 *
 */
void setup_tim3()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 48000 - 1; // 48 MHz / 48000 = 1 kHz
    TIM3->ARR = BUTTON_MAX_PRESS_US;
    TIM3->CNT = 0;
    TIM3->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM3_IRQn);

    // don't enable the timer until we are ready to accept button presses (depends on game state)
}

/**
 * @brief This is the interrupt handler for TIM3. This is invoked if the user holds down the button longer than the
 * maximum press duration
 *
 */
void TIM3_IRQHandler(void)
{
    TIM3->SR &= ~TIM_SR_UIF; // acknowledge the interrupt
    time_out_button();       // time out the button (disables the timer and forces the motor to launch the ball)
}

/**
 * @brief This is the interrupt handler for TIM14. This is invoked if the ultrasonic sensor pulse times out
 *
 */
void TIM14_IRQHandler(void)
{
    TIM14->SR &= ~TIM_SR_UIF;
    time_out_pulse();
}

/**
 * @brief This is the interrupt handler for TIM15. This is invoked if the ultrasonic sensor search times out (ball not detected within a certain window)
 *
 */
void TIM15_IRQHandler(void)
{
    TIM15->SR &= ~TIM_SR_UIF;
    time_out_hcsr04_search();
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

void init_all()
{

    internal_clock();
    init_gpio();
    setup_tim3();
    setup_tim14();
    setup_tim15();
    init_spi1();
    // spi1_init_oled();
    init_display();
}

int main(void)
{
    // internal_clock();
    // init_gpio();
    // setup_tim3();
    // setup_tim14();
    // init_spi1();
    // spi1_init_oled();
    // init_display();
    init_all();

    // spi_write_str("Line 0", 0);
    // spi_write_str("Line 1", 1);
    // spi_write_str("Line 2", 2);
    // spi_write_str("Line 3", 3);
    // game_idle();

    // get_press_duration

    setup_tim17();
    move_to_duty_cycle(0);
    // return 0;

    while (1)
    {
        int dur = get_press_duration();
        char dur_str[20];
        snprintf(dur_str, sizeof(dur_str), "Duration: %d", dur);

        // if (dur > 10)
        // {
        //     spi_write_str("Duration > 10", 3);
        // }
        // else
        // {
        //     spi_write_str("Duration < 10", 3);
        // }
        spi_write_str(dur_str, 3);

        if (dur == 10)
            continue;

        move_to_duty_cycle(dur);
    }

    return 0;
}
