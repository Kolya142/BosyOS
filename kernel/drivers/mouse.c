#include <drivers/keyboard.h>
#include <misc/driverreg.h>
#include <drivers/mouse.h>
#include <drivers/pit.h>
#include <arch/pic.h>
#include <lib/Time.h>
#include <lib/TTY.h>

I32 MouseX = 40, MouseY = 12;
U8 MouseBtn = 0;
U8 MouseCycle = 0;
I8 MousePacket[3];
U0 MouseWait(U8 how);

U0 MouseWrite(U8 data) {
    MouseWait(1);
    POut(0x64, 0xD4);
    MouseWait(1);
    POut(0x60, data);
}
U8 MouseRead() {
    MouseWait(0);
    return PIn(0x60);
}
U0 MouseWait(U8 how) {
    U16 timeout = 100000;
    if (!how) { // Output bit
        while (--timeout) {
            if ((PIn(0x64) & 1) == 0)
                return;
        }
        return;
    }
    else { // Input bit
        while (--timeout) {
            if ((PIn(0x64) & 2) == 2)
                return;
        }
        return;
    }
}

#define WAITI MouseWait(1)
#define WAITO MouseWait(0)

__attribute__((naked)) U0 MouseUpdate() {
    asmv("pushf\npusha");

    if (!(PIn(0x64) & 1)) {
        POut(0x20, 0x20);
        asmv("popa\npopf\niret");
        return;
    }
    PrintF("mouse: %1X\n", PIn(0x60));
    // TODO: Make mouse Update

    POut(0x20, 0x20);
    asmv("popa\npopf\niret");
}

static U0 MSDriverHandler(U32 id, U32 *value) {
    switch (id)
    {
    case 0:
        value[0] = MouseX;
        value[1] = MouseY;
        value[2] = MouseBtn;
        break;
    
    default:
        break;
    }
}

U0 MouseInit() {
    U8 ack;

    POut(0x64, 0xA8); // Enable PS/2 second port

    WAITO; // Enable PS/2 second port interrupts
    POut(0x64, 0x20);
    WAITI;
    ack = PIn(0x60);
    ack |= 1 << 1;
    WAITO;
    POut(0x64, 0x60);
    POut(0x60, ack);

    WAITO; // Enable mouse
    POut(0x64, 0xD4);
    POut(0x60, 0xF4);
    WAITI;
    ack = MouseRead();
    if (ack == 0xFA) {
        WAITO;
        MouseWrite(0xF2);
        WAITI;
        ack = PIn(0x60);
        PrintF("Mouse Driver initialized\nMouse ack: %x\n", ack);
        WAITI;
        ack = PIn(0x60);
        PrintF("Mouse type: %x\n", ack);
    }
}