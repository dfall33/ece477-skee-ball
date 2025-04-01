#include "stm32f0xx.h"
#include <math.h> // for M_PI
#include <stdint.h>
#include <stdio.h>
#include "ultrasonic.h"
#include "button.h"
#include "nhd_0440az.h"
#include "motor_control.h"
#include "joystick.h"
#include "debug.h"
// #include "game.h"
extern game();
extern game_state;

#define PULSE_TIMEOUT_US 30000
#define PULSE_THRESHOLD_US 200

extern void setup_tim17();
extern int play_sound(); 

volatile int detected_streak = 0;

/* ----- Function Declarations ----- */
void nano_wait(int);
void micro_wait(int);




void setup_tim3();
void setup_tim14();
void setup_tim15();


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

    // if currently in idle state, transition to active state (start a game session)
    if (game_state == 0) // 0 = IDLE, see game.h typedef
    {
        game_state = 1; // 1 = ACTIVE

        spi_write_str("trig in exti4_15", 3); 
        return;
    }

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


int main(void)
{
    internal_clock();

    setup_debug_ports(); 
    flash_leds(); 

    setup_display(); 
    test_display(); 

    setup_adc();
    setup_tim16();
    init_tim2();

    setup_ultrasonic_ports();
    setup_tim14(); 
    setup_tim15();


    clear_display();

    // while (1)
    // {
    //     int index = search_hcsr04(1); 
    //     if (index != BALL_NOT_FOUND) // -1 = not found, otherwise, gives index of the sensor that found the ball
    //     {
    //         led_high(index); 
    //         // micro_wait(1000000);
    //         micro_wait(1000000); 
    //         micro_wait(1000000); 
    //         micro_wait(1000000); 
    //         led_off(); 
    //     }
    //     else
    //     {
    //         flash_leds(); 
    //         flash_leds(); 
    //         led_off(); 
    //         micro_wait(1000000);
    //         micro_wait(1000000);
    //         micro_wait(1000000);
    //     }
    // }

    
    game(); // start the game loop

    return 0;
}
