#include <misc/driverreg.h>
#include <kernel/KPanic.h>
#include <drivers/disk/ide.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>

static Bool is_first_atapi;
static Bool is_second_atapi;

Bool ATARead(Bool slave, Ptr buf, U32 start, U8 count) {
    if (!slave && is_first_atapi) {
        return ATAPIRead(False, buf, start / 4);
    }
    if (slave && is_second_atapi) {
        return ATAPIRead(True, buf, start / 4);
    }
    POut(0x01F6, (slave ? 0xF0 : 0xE0) | ((start >> 24) & 15));
    POut(0x01F2, count);
    POut(0x01F3, start & 0xff);
    POut(0x01F4, (start >> 8) & 0xff);
    POut(0x01F5, (start >> 16) & 0xff);
    POut(0x01F7, 0x20);

    Sleep(1);
    U32 timeout = 1000000;
    while (PIn(0x01F7) & 0x80 && --timeout);    // Wait BUSY
    if (!timeout) {
        return False;
    }

    if (PIn(0x01F7) & 1) {
        PrintF("Try access LBA %x\n", start);
        return False;
    }

    U8 *buf8 = buf;

    for (U8 sec = 0; sec < count; ++sec) {
        timeout = 1000000;
        while (!(PIn(0x1F7) & 0x08)) {
            if (PIn(0x1F7) & 1) { return False; }
            if (!--timeout) { return False; }
        }

        for (U32 i = 0; i < 256; i++) {
            U16 word = PIn16(0x01F0);
            buf8[sec * 512 + (i * 2)] = word & 0xff;
            buf8[sec * 512 + (i * 2) + 1] = (word >> 8) & 0xff;
        }
    }

    return True;
}
Bool ATAWrite(Bool slave, Ptr buf, U32 start, U8 count) {
    if (!slave && is_first_atapi) {
        return False;
    }
    if (slave && is_second_atapi) {
        return False;
    }
    POut(0x01F6, (slave ? 0xF0 : 0xE0) | ((start >> 24) & 15));
    POut(0x01F2, count);
    POut(0x01F3, start & 0xFF);
    POut(0x01F4, (start >> 8) & 0xFF);
    POut(0x01F5, (start >> 16) & 0xFF);
    
    Sleep(1);
    U32 timeout = 1000000;
    while (PIn(0x01F7) & 0x80 && --timeout);    // Wait BUSY
    if (!timeout) {
        return False;
    }

    POut(0x01F7, 0x30);

    if (PIn(0x01F7) & 1) {
        PrintF("Try access LBA %x\n", start);
        return False;
    }
    U8 *buf8 = buf;
    for (U8 sec = 0; sec < count; ++sec) {
        timeout = 1000000;
        while (!(PIn(0x01F7) & 0x08) && --timeout);    // Wait DRQ
        if (!timeout) {
            return False;
        }
        for (U32 i = 0; i < 256; ++i) {
            if (buf) {
                U16 word = buf8[sec * 512 + (i * 2)] | (buf8[sec * 512 + (i * 2) + 1] << 8);
                POut16(0x1F0, word);
            }
            else {
                POut16(0x1F0, 0);
            }
        }
    }

    while (PIn(0x01F7) & 0x80);    // Wait BUSY
    POut(0x01F7, 0xE7);
    while (PIn(0x01F7) & 0x80);    // Wait BUSY

    return True;
}

U0 IDEInit() {
    U32 timeout;

    U16 identify[256];

    POut(0x01F6, 0xA0);
    POut(0x80, 0); // Wait
    POut(0x01F7, 0xEC);
    POut(0x80, 0); // Wait

    timeout = 100000;

    while ((PIn(0x1F7) & 0x80) && --timeout);
    if (timeout) {
        for (int i = 0; i < 256; ++i) {
            identify[i] = PIn16(0x1F0);
        }

        if (identify[0] & 0x8000) {
            is_first_atapi = True;
            PrintF("first disk is $!Acdrom$!F\n");
        }
        else {
            PrintF("first disk is $!Bdrive$!F\n");
        }
    }


    POut(0x01F6, 0xB0);
    POut(0x80, 0); // Wait
    POut(0x01F7, 0xEC);
    POut(0x80, 0); // Wait

    timeout = 100000;

    while ((PIn(0x1F7) & 0x80) && --timeout);

    if (timeout) {

        for (int i = 0; i < 256; ++i) {
            identify[i] = PIn16(0x1F0);
        }

        if (identify[0] & 0x8000) {
            is_second_atapi = True;
            PrintF("second disk is $!Acdrom$!F\n");
        }
        else {
            PrintF("second disk is $!Bdrive$!F\n");
        }
    }
}