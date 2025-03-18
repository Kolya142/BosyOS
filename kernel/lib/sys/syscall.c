#include <kernel/KDogWatch.h>
#include <misc/driverreg.h>
#include <kernel/KPanic.h>
#include <lib/sys/syscall.h>
#include <lib/memory/MemLib.h>
#include <lib/IO/TTY.h>
#include <arch/x86/sys/idt.h>
#include <arch/x86/cpu/io.h>

Ptr SysCallT[400] = {0};
U0 SysCallSet(Ptr func, U16 addr) {
    SysCallT[addr] = func;
}
INT_DEF(SysCallInt) {
    INT_START;
    U16 call = regs->eax;
    // TTYSwitch(TTYC_SER);
    // DriverCall(0xa3f13d05, 0x2eb0f0f, 0, (U32*)"\n");
    // PrintF("#Syscall %p %p %p#", regs->eax, regs->esi, regs->edi);
    // DriverCall(0xa3f13d05, 0x2eb0f0f, 0, (U32*)"\n");
    // TTYSwitch(TTYC_RES);
    if (!SysCallT[call]) {
        PrintF("\nInvalid Syscall %p %p %p\n", regs->eax, regs->ebx, regs->ecx);
        KDogWatchLog("Invalid SysCall", False);
        goto end;
    }
    U0 (*sys)(INTRegs3 *regs) = SysCallT[call];
    sys(regs);
    end:
    INT_RETURN;
}
U0 SysCallInit() {
    MemSet(SysCallT, 0, sizeof(SysCallT));
    IDTSet(0x78, SysCallInt, 0x08, 0x8E); // Ring0
    IDTSet(0x80, SysCallInt, 0x08, 0xEE); // Ring3
}