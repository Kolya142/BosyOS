#pragma once
#include <lib/Types.h>
#include <arch/i386/cpu.h>

typedef struct SystemTimeS {
    U16 year;
    U8 month;
    U8 day;
    U8 hour;
    U8 minute;
    U8 second;
} SystemTimeS;

extern SystemTimeS SystemTime;

U0 RTCUpdate();
