#include <drivers/serial.h>
#include <misc/driverreg.h>


static U0 SerialDriverHandler(U32 id, U32 *value) {
    switch (id) {
        case 0:
            SerialWrite(*((U8*)value));
            break;
        case 1:
            *((U8*)value) = SerialRead();
            break;
        case 2:
            *((U8*)value) = SerialBufState();
            break;
        case 3:
            SerialSetSpeed(*((U8*)value));
            break;
        default:
            *value = 0;
    }
}

U0 SerialInit() {
    DriverReg(0xa3f13d05, 0x2eb0f0f, SerialDriverHandler, "com");

    POut(COM1 + 1, 0x00);
    POut(COM1 + 3, 0x80);

    POut(COM1 + 0, SERIAL_115200);
    POut(COM1 + 1, 0x00);

    POut(COM1 + 3, 0x03);
    POut(COM1 + 2, 0xC7);
    POut(COM1 + 4, 0x0B);
}

Byte SerialRead() {
    while (!(PIn(COM1 + 5) & 1));
    return PIn(COM1);
}

U0 SerialWrite(Byte byte) {
    while (!(PIn(COM1 + 5) & 0x20));
    POut(COM1, byte);
}

U32 SerialBufState() {
    return PIn(COM1 + 5);
}

U0 SerialSetSpeed(U8 bod) {
    POut(COM1 + 3, 0x80);
    POut(COM1 + 0, bod);
    POut(COM1 + 1, 0x00);
    POut(COM1 + 3, 0x03);
}