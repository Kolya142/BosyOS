#include <drivers/controllers/ps2.h>
#include <arch/i386/cpu.h>


U0 PS2Wait(U8 how) {
    U32 timeout = 100*1000*1000; // TODO: Replace with sleep
    if (!how) { // Output bit
        while (--timeout) {
            if ((PIn(0x64) & 1) == 1)
                return;
        }
        return;
    }
    else { // Input bit
        while (--timeout) {
            if ((PIn(0x64) & 2) == 0)
                return;
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
}
U8 PS2Read() {
    PS2Wait(0);
    U8 byte = PIn(0x60);
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

    POut(0x64, 0x20);
    U8 status = PIn(0x60);
    POut(0x64, 0x60);
    POut(0x60, status | 0b11);

    POut(0x64, 0xAE);
    if (status & 0x20) {
        POut(0x64, 0xA8);
    }
}