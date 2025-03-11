#include <kernel/KPanic.h>
#include <drivers/input/ps2.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>

U0 PS2Wait(U8 how) {
    U16 timeout = 100;
    if (!how) { // Output bit
        while (--timeout) {
            if ((PIn(0x64) & 1) == 1)
                return;
            SleepM(1);
        }
        return;
    }
    else { // Input bit
        while (--timeout) {
            if ((PIn(0x64) & 2) == 0)
                return;
            SleepM(1);
        }
        return;
    }
}

U0 PS2Clean() {
    while (PIn(0x64) & 1) {
        PIn(0x60);
    }
}
U0 PS2Write(U8 cmd, U8 byte) {
    PS2Wait(1);
    POut(0x64, cmd);
    PS2Wait(1);
    POut(0x60, byte);
    SerialPrintF("PS/2Write: %1X -> %1X", cmd, byte);
}
U8 PS2Read() {
    PS2Wait(0);
    U8 byte = PIn(0x60);
    SerialPrintF("PS/2Read: %1X", byte);
    return byte;
}

U0 PS2Init() {
    POut(0x64, 0xAD);
    POut(0x64, 0xA7);

    PS2Clean();

    PS2Wait(1);
    POut(0x64, 0xAA);
    PS2Wait(0);
    U8 test = PIn(0x60);
    if (test == 0xFC) {
        KPanic("PS2 POST Failed", True);
    }
    else if (test != 0x55) {
        KPanic("PS2 POST Unknown", True);
    }

    POut(0x64, 0x20);
    U8 status = PIn(0x60);
    PrintF("PS/2 status: $!B%1b$!F\n", status);
    POut(0x64, 0x60);
    POut(0x60, status | 0b11);

    POut(0x64, 0xAE);
    if (status & 0x20) {
        POut(0x64, 0xA8);
    }
}