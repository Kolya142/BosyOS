#include <kernel/clock.h>
#include <lib/shash.h>
#pragma once

typedef struct GUIDM {
    unsigned short dd;
    unsigned short dm;
    unsigned short de;
    unsigned short df;
} GUIDM;

GUIDM GUIDMUnicue();