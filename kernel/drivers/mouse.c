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

U0 MouseWrite(U16 port, U8 data) {
    while (PIn(0x64) & 2);
    POut(port, data);
}
U8 MouseRead(U16 port) {
    while (!(PIn(0x64) & 1));
    return PIn(port);
}

__attribute__((naked)) U0 MouseUpdate() {
    asmv("pushf\npusha");

    if (!(PIn(0x64) & 1)) {
        asmv("popa\npopf\niret");
        return;
    }

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
    MouseWrite(0x64, 0xD4);
    MouseWrite(0x60, 0xF4);
    SleepM(100);
    U8 ack = MouseRead(0x60);
    if (ack == 0xFA) {
        MouseWrite(0x60, 0xF2);
        PrintF("Mouse initialized\nMouse id: 0x%x\n", MouseRead(0x60));
    }
    else {
        PrintF("Mouse initialization failed\n");
        // IDTSet(0x2C, (Ptr)MouseUpdate, 8, 0x8E);
        DriverReg(0x58f8d530, 0xfffe784f, MSDriverHandler);
    }
}
