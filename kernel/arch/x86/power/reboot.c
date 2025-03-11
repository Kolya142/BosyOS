#include <kernel/KPanic.h>
#include <arch/x86/cpu/io.h>

U0 PowerReboot() {
    // try 0
    POut(0x64, 0xFE);
    // try 1
    for (I32 i = 0; i < 1000; i++) {
        POut(0x92, 0x01);
    }
    for (I32 i = 0; i < 100; i++) asm("pause");
    // try 2
    POut(0x64, 0xFE);
    for (I32 i = 0; i < 100; i++) asm("pause");
    // try 3
    POut(0x0CF9, 0x06);
    for (I32 i = 0; i < 100; i++) asm("pause");
    // error
    KPanic("System can't reboot", False);
}