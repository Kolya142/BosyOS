#pragma once
#include <drivers/pit.h>

// Sleep for x milliseconds
U32 SleepM(U32 millis);

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
U8 BCDToBin(U8 bcd);
U0 RTCUpdate();