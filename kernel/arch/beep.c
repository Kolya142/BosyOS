#include <arch/beep.h>

U0 Beep(U16 dur) {
    POut(0x61, PIn(0x61) | 3);
    MSleep(dur);
    POut(0x61, PIn(0x61) & ~3);
}