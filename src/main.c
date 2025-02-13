#include "stm32f0xx.h"
#include <math.h> // for M_PI
#include <stdint.h>
#include <stdio.h>

void nano_wait(int);

//=============================================================================
// Part 1: 7-segment display update with DMA
//=============================================================================

// 16-bits per digit.
// The most significant 8 bits are the digit number.
// The least significant 8 bits are the segments to illuminate.
// uint16_t msg[8] = {0x0000, 0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700};
// extern const char font[];
// // Print an 8-character string on the 8 digits
// void print(const char str[]);
// // Print a floating-point value.
// void printfloat(float f);

//============================================================================
// setup_dma() + enable_dma()

//============================================================================
// init_tim15()
//============================================================================
// Part 2: Debounced keypad scanning.
//=============================================================================
// demonstrate get_key_event()

//============================================================================
// Varables for boxcar averaging.
//============================================================================
//============================================================================
// All the things you need to test your subroutines.
//============================================================================
int main(void)
{
    internal_clock();

    printf("Hello, World!\n");

    // Comment this for-loop before you demo part 1!
    // Uncomment this loop to see if "ECE 362" is displayed on LEDs.
    // for (;;) {
    //     asm("wfi");
    // }
    // End of for loop
    // Have fun.
}
