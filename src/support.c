#include "stm32f0xx.h"
#include <string.h> // for memmove()
#include <stdio.h>  // for memmove()

void nano_wait(unsigned int n)
{
    asm("        mov r0,%0\n"
        "repeat: sub r0,#83\n"
        "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void micro_wait(unsigned int n)
{
    nano_wait(n * 1000);
}
