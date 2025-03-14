#pragma once
#include <drivers/time/rtc.h>
#include <drivers/time/pit.h>

// Sleep for x milliseconds
U32 Sleep(U32 millis);

U8 RTCRead(U8 reg);
U8 BCDToBin(U8 bcd);
U0 RTCUpdate();