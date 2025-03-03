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
    if (!how) {
        while (--timeout) {
            if ((PIn(0x64) & 1) == 1)
                return;
        }
        return;
    }
    else {
        while (--timeout) {
            if ((PIn(0x64) & 2) == 0)
                return;
        }
        return;
    }
}

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
    PrintF("Initializing mouse...\n");
}