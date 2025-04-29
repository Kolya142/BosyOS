#include <drivers/time/pit.h>

volatile U32 PITTime = 0;

INT_DEF(PITHandler) {
    PITTime += 1000/16;
}

U0 PITInit() {
    U16 div = 1193182 / 60;
    IDTSet(32, PITHandler, 0x08, 0x8E);

    POut(0x43, 0x36);
    POut(0x40, div & 0xff);
    POut(0x40, (div>>8) & 0xff);
    POut(0x21, PIn(0x21) & ~1);
}