#include "button.h"
extern volatile int game_state;
extern void progress_bar();
volatile int8_t button_press_progress; // int [1, 10] to indicate progress of button press as percent of max.

/* ----- Control Flags -----  */
volatile int button_released = 0;
volatile int button_timed_out = 0;
volatile int button_pressed = 0;
volatile int button_pressable = 0;

void EXTI4_15_IRQHandler(void)
{
    // acknowledge the interrupt
    EXTI->PR = EXTI_PR_PR7; // Clear the pending bit for line 7 (PB7)

    // if currently in idle state, transition to active state (start a game session)
    if (game_state == 0) // 0 = IDLE, see game.h typedef
    {
        game_state = 1; // 1 = ACTIVE
        button_pressable = 0;

        // disable button interrupt (will enable it again a bit later in game_active())
        NVIC_DisableIRQ(EXTI4_15_IRQn); // Disable the button interrupt to prevent further interrupts while transitioning
        return;
    }

    if (game_state == 1 && !button_pressable)
    {
        button_pressable = 1;
        return;
    }

    // if the input data register is high, then the button is pressed and being held down,
    // so start the button press timer (will time out after BUTTON_MAX_PRESS_US, defined in src/button.h)
    if (!button_pressable)
        return;

    if (GPIOB->IDR & GPIO_IDR_7) // Check if PB7 is high (button pressed)
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
 * @brief This function is called when the button is pressed. It starts the button press timer and transitions to the button press state.
 * 
 */
void start_button_press()
{

    // if we are here, then we are in the active state and just pressed the button, so transition to the button press state
    game_state = 2; // go to button press state
    start_button_timer();
    button_pressed = 1;
    button_released = 0;
    button_timed_out = 0;
}

/**
 * @brief This function is called when the button is released. It stops the button press timer and transitions to the button release state.
 * 
 */
void stop_button_press()
{
    button_pressed = 0;
    button_released = 1;
    button_timed_out = 0;
    stop_button_timer();
}

/**
 * @brief This function is called when the button times out. It stops the button press timer and transitions to the button timeout state.
 * 
 */
void time_out_button()
{
    button_pressed = 0;
    button_released = 0;
    button_timed_out = 1;
    stop_button_timer();
}

/**
 * @brief This function starts the button timer. It is called when the button is pressed.
 * 
 */
void start_button_timer()
{
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CNT = 0;
    TIM3->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief This function stops the button timer. It is called when the button is released or times out.
 * 
 */
void stop_button_timer()
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->DIER &= ~TIM_DIER_UIE;
}

/**
 * @brief This function disables the button interrupt. It is called when the button is pressed or released.
 * 
 */
void disable_button_interrupt()
{
    // disable the external interrupt associated with the button
    NVIC_DisableIRQ(EXTI4_15_IRQn);
}

/**
 * @brief This function enables the button interrupt. It is called when the button is released.
 * 
 */
void enable_button_interrupt()
{
    // enable the external interrupt associated with the button
    NVIC_EnableIRQ(EXTI4_15_IRQn); // enable the interrupt for the button on PC13
}

int get_press_duration()
{

    // wait for the button to be released, or for the press to time out (max press duration exceeded)
    while (!button_released && !button_timed_out)
        ;

    button_timed_out = 0; // reset timeout flag
    button_released = 0;  // reset released flag
    button_pressed = 0;

    int8_t ret = button_press_progress;

    button_press_progress = 0; // reset the progress for next button press
    return ret;
}

/**
 * @brief This function initializes the button external interrupt. This is called in main.c to setup the button interrupt.
 * 
 */
void init_button_exti()
{
    // enable clock for syscfg
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

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

/**
 * @brief Setup the GPIO port for the button. This is called in main.c to setup the button GPIO port.
 * 
 */
void init_button_gpio()
{
    // enable clock for GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~(GPIO_MODER_MODER7); // Clear mode for PB7
}

void setup_tim3()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
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
        progress_bar(button_press_progress, 1); // Update the progress bar on line 1 of the display
    }

    else
    {
        button_press_progress = BUTTON_MAX_VALUE; // cap it at max value
        progress_bar(button_press_progress, 1);   // Update the progress bar on line 1 of the display
        time_out_button();
    }
}