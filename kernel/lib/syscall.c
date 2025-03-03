#include <kernel/KDogWatch.h>
#include <misc/driverreg.h>
#include <kernel/KPanic.h>
#include <misc/syscall.h>
#include <lib/MemLib.h>
#include <lib/TTY.h>
#include <arch/idt.h>
#include <arch/io.h>

Ptr SysCallT[256] = {0};
U0 SysCallSet(Ptr func, U8 addr) {
    SysCallT[addr] = func;
}
INT_DEF(SysCallInt) {
    INT_START;
    U8 call = regs->eax;
    // TTYSwitch(TTYC_SER);
    // DriverCall(0xa3f13d05, 0x2eb0f0f, 0, (U32*)"\n");
    // PrintF("#Syscall %p %p %p#", regs->eax, regs->esi, regs->edi);
    // DriverCall(0xa3f13d05, 0x2eb0f0f, 0, (U32*)"\n");
    // TTYSwitch(TTYC_RES);
    if (!SysCallT[call]) {
        KDogWatchLog("Invalid SysCall", False);
        PrintF("Syscall %p %p %p\n", regs->eax, regs->esi, regs->edi);
        goto end;
    }
    U0 (*sys)(INTRegs *regs) = SysCallT[call];
    sys(regs);
    end:
    INT_RETURN;
}
U0 SysCallInit() {
    MemSet(SysCallT, 0, sizeof(SysCallT));
    IDTSet(0x78, SysCallInt, 0x08, 0x8E); // Ring0
    IDTSet(0x80, SysCallInt, 0x08, 0xEE); // Ring3
}