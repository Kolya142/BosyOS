#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <misc/syscall.h>
#include <lib/MemLib.h>
#include <arch/idt.h>
#include <arch/io.h>

Ptr SysCallT[256] = {0};
U0 SysCallSet(Ptr func, U8 addr) {
    SysCallT[addr] = func;
}
INT_DEF(SysCallInt) {
    INT_START;
    U8 call;
    asmV("movb %%al, %0": "=r"(call));
    if (!SysCallT[call]) {
        KDogWatchLog("Invalid SysCall", False);
        goto end;
    }
    U0 (*sys)() = SysCallT[call];
    sys();
    end:
    INT_RETURN;
}
U0 SysCallInit() {
    MemSet(SysCallT, 0, sizeof(SysCallT));
    IDTSet(0x78, SysCallInt, 0x08, 0x8E); // Ring0
    IDTSet(0x80, SysCallInt, 0x1B, 0xEF); // Ring3
}