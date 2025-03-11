#include <drivers/input/keyboard.h>
#include <misc/driverreg.h>
#include <drivers/input/mouse.h>
#include <drivers/time/pit.h>
#include <arch/x86/cpu/pic.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>

I32 MouseVX = 40*2, MouseVY = 12*2;
I32 MouseX = 40, MouseY = 12;
U8 MouseBtn = 0;
volatile U8 MouseCycle = 0;
volatile I8 MousePacket[3];
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
U0 MouseClean() {
    while (PIn(0x64) & 1) {
        PIn(0x60);
    }
}
U0 MouseWait(U8 how) {
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

__attribute__((naked)) U0 MouseUpdate() {
    U32 esp;
    asmv("pusha\npushf\nmov %%esp, %0" :: "r"(esp));
    while ((PIn(0x64) & 1)) {
        if (!(PIn(0x64) & (1 << 5))) {
            goto end;
        }

        I8 data = PIn(0x60);
        
        switch (MouseCycle) {
            case 0:
                if (!(data & 8)) {
                    MouseCycle = 0;
                    break;
                }
                MousePacket[MouseCycle] = data;
                ++MouseCycle;
            break;
            case 1:
                MousePacket[MouseCycle] = data;
                ++MouseCycle;
            break;
            case 2:
                MousePacket[MouseCycle] = data;
                MouseCycle = 0;

                MouseBtn = MousePacket[0] & 0b111;
                MouseVX += MousePacket[1];
                MouseVY -= MousePacket[2];

                if (MouseVX < 0) MouseVX = 0;
                if (MouseVY < 0) MouseVY = 0;
                if (MouseVX > 319*2) MouseVX = 319*2;
                if (MouseVY > 199*2) MouseVY = 199*2;
                MouseX = MouseVX / 2;
                MouseY = MouseVY / 2;
            break;
        }
    }
    end:
    POut(0x20, 0x20);
    POut(0xA0, 0x20);
    asmv("mov %0, %%esp\npopf\npopa\niret" :: "r"(esp));
    return;
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

static U0 MouseEnable() {
    PS2Clean();

    PS2Wait(1);
    POut(0x64, 0xA8);

    PS2Wait(1);
    POut(0x64, 0xE6);

    PS2Wait(1);
    POut(0x64, 0x20);
    U8 conf = PS2Read();
    conf |= 0b10;
    PS2Write(0x60, conf);

    PS2Write(0xD4, 0xF4);
    U8 ack = PS2Read();

    PS2Clean();

    if (ack != 0xFA) {
        PrintF("Mouse initialization failed! ACK: %X\n", ack);
    }
}

U0 MouseInit() {
    U8 ack;
    PS2Clean();

    PICClearMask(12);
    
    MouseEnable();
    
    IDTSet(0x2C, MouseUpdate, 0x08, 0x8E);
}
