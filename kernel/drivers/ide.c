#include <kernel/KPanic.h>
#include <drivers/ide.h>
#include <lib/Time.h>
#include <lib/TTY.h>

U0 ATARead(Ptr buf, U32 start, U8 count) {
    POut(0x01F6, 0xE0 | ((start >> 24) & 15));
    POut(0x01F2, count);
    POut(0x01F3, start & 0xff);
    POut(0x01F4, (start >> 8) & 0xff);
    POut(0x01F5, (start >> 16) & 0xff);
    POut(0x01F7, 0x20);

    while (PIn(0x01F7) & 0x80);    // Wait BUSY
    if (PIn(0x01F7) & 1) {
        KPanic("ATA ERROR: Disk Busy wait failed", True);
    }

    U8 *buf8 = buf;

    for (U8 sec = 0; sec < count; ++sec) {
        while (!(PIn(0x1F7) & 0x08)) {
            if (PIn(0x1F7) & 1) { KPanic("ATA ERROR: BSY Failed!", True); }
            if (PIn(0x1F7) & 0x80) { KPanic("ATA ERROR: BSY stuck!", True); }
        }

        for (U32 i = 0; i < 256; i++) {
            U16 word = PIn16(0x01F0);
            buf8[sec * 512 + (i * 2)] = word & 0xff;
            buf8[sec * 512 + (i * 2) + 1] = (word >> 8) & 0xff;
        }
        while (PIn(0x1F7) & 0x80);
    }
}
U0 ATAWrite(Ptr buf, U32 start, U8 count) {
    POut(0x01F6, 0xE0 | ((start >> 24) & 15));
    POut(0x01F2, count);
    POut(0x01F3, start & 0xFF);
    POut(0x01F4, (start >> 8) & 0xFF);
    POut(0x01F5, (start >> 16) & 0xFF);
    POut(0x01F7, 0x30);

    while (PIn(0x01F7) & 0x80);    // Wait BUSY
    if (PIn(0x01F7) & 1) {
        KPanic("ATA ERROR: Disk Busy wait failed", True);
    }
    U8 *buf8 = buf;
    for (U8 sec = 0; sec < count; ++sec) {
        while (!(PIn(0x01F7) & 0x08)); // Wait DRQ
        for (U32 i = 0; i < 256; ++i) {
            U16 word = buf8[sec * 512 + (i * 2)] | (buf8[sec * 512 + (i * 2) + 1] << 8);
            POut16(0x1F0, word);
        }
    }

    while (PIn(0x01F7) & 0x80);    // Wait BUSY
    POut(0x01F7, 0xE7);
    while (PIn(0x01F7) & 0x80);    // Wait BUSY
}