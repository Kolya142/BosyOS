#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/pit.h>
#include <arch/pic.h>
#include <lib/TTY.h>

I32 MouseX = 40, MouseY = 12;
U8 MouseBtn = 0;
U8 MouseCycle = 0;
I8 MousePacket[3];

extern volatile KBStateS KBState;

U0 MouseWait(Bool type) {
    U32 timeout = 100000;
    if (!type) {  
        while (--timeout) {
            if ((PIn(0x64) & 1) == 1) return;
        }
        return;
    }
    else {
        while (--timeout) {
            if (!(PIn(0x64) & 2)) return;
        }
    }
}

U0 MouseWrite(U8 data) {
    MouseWait(True);
    POut(0x64, 0xD4);  
    MouseWait(True);
    POut(0x60, data);
}

U8 MouseRead() {
    MouseWait(False);
    return PIn(0x60);
}

INT_DEF(MouseUpdate) {
    asm volatile("pusha\npushf");
    PrintF("Mouse event");
    switch (MouseCycle) {
        case 0:
            MousePacket[0] = PIn(0x60);
            ++MouseCycle;
            break;
        case 1:
            MousePacket[1] = PIn(0x60);
            ++MouseCycle;
            break;
        case 2:
            MousePacket[2] = PIn(0x60);
            MouseX += MousePacket[1];
            MouseY -= MousePacket[2];
            MouseBtn = MousePacket[0] & 7;
            MouseCycle = 0;
            break;
    }
    while (PIn(0x64) & 1) {
        U8 garbage = PIn(0x60);
        PrintF("Cleared garbage byte: 0x%C", garbage);
    }
    POut(0xA0, 0x20);
    POut(0x20, 0x20);
    asmv("popf\npopa\niret");
}

U0 MouseInit() {
    U32 time = PITTime;
    POut(0x21, 0);
    POut(0xA1, 0);
    PrintF("Mouse init start\n");

    while (PIn(0x64) & 1) PIn(0x60);

    MouseWait(True);     // Auxiliiary
    POut(0x64, 0xA8);  

    MouseWait(True);     // INTS
    POut(0x64, 0x20);
    MouseWait(False);
    U8 status = PIn(0x60);
    status |= 2;
    status |= 1;
    PrintF("Mouse status: %b\n", status);
    MouseWait(True);
    POut(0x64, 0x60);
    MouseWait(True);
    POut(0x60, status);
    PrintF("MouseInit: Updated PS/2 status: %b\n", PIn(0x60));
    MouseWrite(0xF6); // Reset
    if (MouseRead() != 0xFA) {
        PrintF("Mouse reset failed\n");
        return;
    }

    MouseWrite(0xF4); // Enable
    if (MouseRead() != 0xFA) {
        PrintF("Mouse init failed\n");
        return;
    }
    MouseWait(False);
    PIn(0x60);

    PICClearMask(1);
    PICClearMask(12);
    IDTSet(0x2C, (Ptr)MouseUpdate, 8, 0x8E);
    U8 irq_state = PIn(0x21);
    irq_state = PIn(0xA1);
    PrintF("IRQ State (PIC Master): %x\n", irq_state);
    PrintF("IRQ State (PIC Slave): %x\n", irq_state);
    
    MouseWait(False);

    PrintF("Mouse initialized successfully for time: %xms\n", PITTime - time);
}