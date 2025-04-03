#include "button.h"
// #include "game.h"

// extern game_active();

extern void led_high(int); 
extern void led_low(int); 
extern volatile int game_state;
extern void progress_bar(); 

volatile int8_t button_press_progress; // int [1, 10] to indicate progress of button press as percent of max.

#define BUTTON_MAX_VALUE 10 

volatile int button_released = 0;
volatile int button_timed_out = 0;
volatile int button_pressed = 0;

// extern game_state; // get the game state variable from game.c

// external interrupt handler for push button (pc13)
void EXTI4_15_IRQHandler(void)
{
    // acknowledge the interrupt
    // EXTI->PR |= EXTI_PR_PR13;
    EXTI->PR = EXTI_PR_PR7; // Clear the pending bit for line 7 (PB7)

    // // if currently in idle state, transition to active state (start a game session)
    // if (game_state == 0) // 0 = IDLE, see game.h typedef
    // {
    //     game_state = 1; // 1 = ACTIVE

    //     // spi_write_str("trig in exti4_15", 3); 
    //     return;
    // }

    // if the input data register is high, then the button is pressed and being held down,
    // so start the button press timer (will time out after BUTTON_MAX_PRESS_US, defined in src/button.h)
    // if (GPIOC->IDR & GPIO_IDR_13)
    // {
    //     start_button_press();
    // }
    if (GPIOB->IDR & GPIO_IDR_7) // Check if PB7 is high (button pressed)
    {
        start_button_press();
        led_high(0); // Turn on LED 1 to indicate button press
    }

    // if the input data register is low, then the button has been released
    // so stop the button press timer and do whatever is necessary
    else
    {
        stop_button_press();
        led_low(0); 
    }
}


void start_button_press()
{

    // if we are here, then we are in the active state and just pressed the button, so transition to the button press state
    game_state = 2; // go to button press state

    start_button_timer();
    button_pressed = 1;
    button_released = 0;
    button_timed_out = 0;

    // game_active();
}

void stop_button_press()
{
    // uint32_t duration_us = TIM3->CNT;

    // if (duration_us > BUTTON_MAX_PRESS_US)
    //     return BUTTON_MAX_PRESS_US;
    // else
    //     return duration_us;

    button_pressed = 0;
    button_released = 1;
    button_timed_out = 0;
    stop_button_timer();
}

void time_out_button()
{
    button_pressed = 0;
    button_released = 0;
    button_timed_out = 1;
    stop_button_timer();
}

void start_button_timer()
{
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CNT = 0;
    TIM3->CR1 |= TIM_CR1_CEN;
}

void stop_button_timer()
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->DIER &= ~TIM_DIER_UIE;
}

void disable_button_interrupt()
{
    // disable the external interrupt associated with the button
    // EXTI->IMR &= ~EXTI_IMR_IM0;
    // NVIC_DisableIRQ(EXTI0_1_IRQn);
    NVIC_DisableIRQ(EXTI4_15_IRQn); 
}

void enable_button_interrupt()
{
    // enable the external interrupt associated with the button
    // EXTI->IMR |= EXTI_IMR_IM0;
    // NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_EnableIRQ(EXTI4_15_IRQn); // enable the interrupt for the button on PC13
}

int get_press_duration()
{

    // wait for the button to be released, or for the press to time out (max press duration exceeded)
    while (!button_released && !button_timed_out)
        ;

    // if (button_released)
    // {
    //     button_released = 0;
    //     button_pressed = 0;
    //     button_timed_out = 0;

    //     // scale the timer counter to an integer between 1 and 10
    //     // (interpoliation of press duration 0us <= t <= BUTTON_MAX_PRESS_US to digital value 1 <= x <= 10)
    //     return (10 * TIM3->CNT) / BUTTON_MAX_PRESS_US;
    // }
    // else
    // {
    //     button_timed_out = 0;
    //     button_pressed = 0;
    //     button_released = 0;
    //     return 10;
    // }

    button_timed_out = 0; // reset timeout flag
    button_released = 0; // reset released flag
    button_pressed = 0; 

    int8_t ret = button_press_progress; 

    button_press_progress = 0; // reset the progress for next button press
    return ret; 
}

// enable exti on pc13
void init_button_exti()
{
    // enable clock for syscfg
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // // set PC13 as external interrupt source
    // SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    // SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

    // // enable interrupt on rising edge and falling edge
    // EXTI->RTSR |= EXTI_RTSR_TR13;
    // EXTI->FTSR |= EXTI_FTSR_TR13;

    // // enable interrupt on line 13
    // EXTI->IMR |= EXTI_IMR_MR13;

    // set PB7 as external interrupt source 
    SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI7; 
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI7_PB; 

    // enable interrupt on rising edge and falling edge
    EXTI->RTSR |= EXTI_RTSR_TR7; // Rising edge trigger for PB7
    EXTI->FTSR |= EXTI_FTSR_TR7; // Falling edge trigger for PB7

    // enable interrupt on line 7
    EXTI->IMR |= EXTI_IMR_MR7; // Enable interrupt for line 7 (PB7)

    // enable the interrupt in the NVIC
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void init_button_gpio()
{
    // enable clock for GPIOC
    // RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    // // set PC13 to input mode
    // GPIOC->MODER &= ~(GPIO_MODER_MODER13);

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; 
    GPIOB->MODER &= ~(GPIO_MODER_MODER7); // Clear mode for PB7
}




void setup_tim3()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    // TIM3->PSC = 48000 - 1; // 48 MHz / 48000 = 1 kHz

    // make 10x faster to poll button for progress bar 
    TIM3->PSC = 4800 - 1; // 48 MHz / 48000 = 1 kHz
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

    if (button_press_progress < BUTTON_MAX_VALUE)
    {
        button_press_progress++; 
        progress_bar( button_press_progress, 1); // Update the progress bar on line 1 of the display

    }
        

    else 
    {
        button_press_progress = BUTTON_MAX_VALUE; // cap it at max value
        progress_bar( button_press_progress, 1); // Update the progress bar on line 1 of the display
        time_out_button(); 
    }

    // time_out_button();       // time out the button (disables the timer and forces the motor to launch the ball)
}