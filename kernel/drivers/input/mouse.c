#include <drivers/input/keyboard.h>
#include <misc/driverreg.h>
#include <drivers/input/mouse.h>
#include <lib/graphics/Graphics.h>
#include <drivers/time/pit.h>
#include <arch/x86/cpu/pic.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>

I32 MouseX = WIDTH/2, MouseY = HEIGHT/2;
U8 MouseBtn = 0;
static volatile U8 MouseCycle = 0;
static volatile I8 MousePacket[3];

__attribute__((naked)) U0 MouseUpdate() {
    U32 esp;
    asmv("pusha\npushf\nmov %%esp, %0" : "=r"(esp));
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
                MouseX += MousePacket[1];
                MouseY -= MousePacket[2];

                if (MouseX < 0) MouseX = 0;
                if (MouseY < 0) MouseY = 0;

                if (MouseX > WIDTH - 1) MouseX = WIDTH - 1;
                if (MouseY > HEIGHT - 1) MouseY = HEIGHT - 1;
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
    PICClearMask(12);
    
    MouseEnable();
    
    IDTSet(0x2C, MouseUpdate, 0x08, 0x8E);
}
