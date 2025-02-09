#include "sleep.h"

static inline unsigned long long rdtsc() {
    U32 lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)hi << 32) | lo;
}

void usleep(U32 millis)
{
    const U32 coff = 10000;
    millis *= coff;
    unsigned long long start = rdtsc();
    // TODO: make using global isr
    while (rdtsc() - start < millis) {
        Asm("pause");
    }
}