#include "halt.h"

void Halt() {
/*
Description:
 Halt is not just an assembler code (wfi or hlt), it is a direct manipulation of time and space
Usage:
 in the kernel: after work, after an error
 in user space: if you want to crush the whole system
 in real life: halt yourself for 5 minutes
Warning:
 in the cpu is a one-way command
*/
    asm("hlt");
    for(;;);
}