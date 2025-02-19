#include <drivers/pit.h>
#include <kernel/KDogWatch.h>

volatile U32 PITTime = 0;

INT_DEF(PITHandler) {
    static volatile U16 timer = 0;
    INT_START;
    PITTime++;
    timer++;
    if (timer) {
        KDogWatchTick();
    }
    INT_RETURN;
}

U0 PITInit() {
    U16 div = PIT_FREQ / 1900;

    POut(0x43, 0x36);
    POut(0x40, div & 0xff);
    POut(0x40, (div>>8) & 0xff);
    POut(0x21, PIn(0x21) & ~1);
    IDTSet(32, PITHandler, 0x08, 0x8E);
    asmv("int $32");
}