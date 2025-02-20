#pragma once
#include <drivers/pit.h>

// Sleep for x milliseconds
U32 MSleep(U32 millis);

typedef struct SystemTimeS {
    U16 year;
    U8 month;
    U8 day;
    U8 hour;
    U8 minute;
    U8 second;
} SystemTimeS;

extern SystemTimeS SystemTime;

U8 RTCRead(U8 reg);
U0 RTCUpdate();