#include "time.h"

void PITInit()
{
    U16 div = PIT_FREQ / 20000;

    outb(0x43, 0x36);
    outb(0x40, div % 0xff);
    outb(0x40, (div>>8) % 0xff);
}