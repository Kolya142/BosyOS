#include <arch/i386/cpu.h>

U0 PICMap() {
    POut(0x20, 0x11);
    POut(0xA0, 0x11);
    
    POut(0x21, 0x20);
    POut(0xA1, 0x28);

    POut(0x21, 0x04);
    POut(0xA1, 0x02);

    POut(0x21, 0x01);
    POut(0xA1, 0x01);

    POut(0x21, 0x00);
    POut(0xA1, 0x00);
}
U0 PICClearMask(U8 irq) {
    U16 port;
    U8 value;

    if (irq < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        irq -= 8;
    }
    value = PIn(port) & ~(1 << irq);
    POut(port, value);
}