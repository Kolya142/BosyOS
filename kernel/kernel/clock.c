#include "clock.h"

U32 SysClock = 0;

void SysTick()
{
    usleep(20);
    SysClock++;
}