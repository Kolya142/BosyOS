#pragma once
#include <lib/Types.h>
#include <arch/io.h>
#define COM1 0x3F8

enum SerialSpeed {
    SERIAL_115200 = 1,
    SERIAL_57600  = 2,
    SERIAL_38400  = 3,
    SERIAL_19200  = 6,
    SERIAL_9600   = 12
};

U0 SerialInit();

Byte SerialRead();

U0 SerialWrite(Byte byte);

U32 SerialBufState();

U0 SerialSetSpeed(U8 bod);