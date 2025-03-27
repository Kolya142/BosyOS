#include <misc/driverreg.h>
#include <kernel/KPanic.h>
#include <drivers/disk/ide.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>

U0 ATARead(Bool slave, Ptr buf, U32 start, U8 count) {
    POut(0x01F6, (slave ? 0xF0 : 0xE0) | ((start >> 24) & 15));
    POut(0x01F2, count);
    POut(0x01F3, start & 0xff);
    POut(0x01F4, (start >> 8) & 0xff);
    POut(0x01F5, (start >> 16) & 0xff);
    POut(0x01F7, 0x20);

    Sleep(1);
    while (PIn(0x01F7) & 0x80);    // Wait BUSY

    if (PIn(0x01F7) & 1) {
        PrintF("Try access LBA %x\n", start);
        KPanic("ATA ERROR: Disk Busy wait failed", True);
    }

    U8 *buf8 = buf;

    for (U8 sec = 0; sec < count; ++sec) {
        U32 timeout = 1000000;
        while (!(PIn(0x1F7) & 0x08)) {
            if (PIn(0x1F7) & 1) { KPanic("ATA ERROR: BSY Failed!", True); }
            if (PIn(0x1F7) & 0x80) { if (!--timeout) { KPanic("ATA ERROR: BSY stuck!", True); } }
        }

        for (U32 i = 0; i < 256; i++) {
            U16 word = PIn16(0x01F0);
            buf8[sec * 512 + (i * 2)] = word & 0xff;
            buf8[sec * 512 + (i * 2) + 1] = (word >> 8) & 0xff;
        }
    }
}
U0 ATAWrite(Bool slave, Ptr buf, U32 start, U8 count) {
    POut(0x01F6, (slave ? 0xF0 : 0xE0) | ((start >> 24) & 15));
    POut(0x01F2, count);
    POut(0x01F3, start & 0xFF);
    POut(0x01F4, (start >> 8) & 0xFF);
    POut(0x01F5, (start >> 16) & 0xFF);
    
    Sleep(1);
    while (PIn(0x01F7) & 0x80);    // Wait BUSY

    POut(0x01F7, 0x30);

    if (PIn(0x01F7) & 1) {
        PrintF("Try access LBA %x\n", start);
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

static U0 IDEDriverHandler(U32 id, U32 *value) {
    IDEAsk *ask = (IDEAsk*)(value);
    if (id == 0) {
        ATARead(0, ask->buf, ask->start, ask->end);
    }
    else if (id == 1) {
        ATAWrite(0, ask->buf, ask->start, ask->end);
    }
}

U0 IDEInit() {
    DriverReg(0x6aa73a10, 0xa8b55cb4, IDEDriverHandler, "ide");
}