#include <drivers/disk/atapi.h>
#include <misc/driverreg.h>
#include <kernel/KPanic.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>

Bool ATAPIRead(Bool slave, Ptr buf, U32 start) {
    POut(0x01F6, slave ? 0xB0 : 0xA0);

    Sleep(1);

    POut(0x01F2, 0);
    POut(0x01F3, 0);
    POut(0x01F4, 0);
    POut(0x01F5, 0);

    POut(0x1F7, 0xA0);

    U32 timeout = 1000000;
    while (PIn(0x01F7) & 0x80 && --timeout);    // Wait BUSY
    if (!timeout) {
        return False;
    }

    timeout = 1000000;
    while (!(PIn(0x1F7) & 0x08) && --timeout);
    if (!timeout) {
        return False;
    }

    Sleep(1);

    U8 packet[12] = {
        0xA8,
        0x00,
        (start >> 24) & 0xFF,
        (start >> 16) & 0xFF,
        (start >>  8) & 0xFF,
        (start >>  0) & 0xFF,
        0x00,
        0x00,
        0x00,
        0x01,
        0x00,
        0x00
    };
    for (int i = 0; i < 6; ++i) {
        U16 *p = (U16*)packet;
        POut16(0x1F0, p[i]);
    }

    timeout = 1000000;
    while ((PIn(0x1F7) & 0x80) && --timeout);
    if (!timeout) return False;

    while (!(PIn(0x1F7) & 0x08));

    if (PIn(0x1F7) & 0x01) return False;

    U8 *buf8 = buf;
    for (U32 i = 0; i < 1024; i++) {
        U16 word = PIn16(0x01F0);
        buf8[i * 2] = word & 0xff;
        buf8[i * 2 + 1] = (word >> 8) & 0xff;
    }

    return True;
}