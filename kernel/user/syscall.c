#include <user/syscall.h>
#include <arch/cpu.h>
#include <arch/io.h>

Ptr SysCallT[256] = {0};
U0 SysCallSet(Ptr func, U8 addr) {
    SysCallT[addr] = func;
}
INT_DEF(SysCallInt) {
    INT_START;
    U8 call;
    asmV("movb %%al, %0": "=r"(call) :: "ax");
    U0 (*sys)() = SysCallT[call];
    sys();
    INT_RETURN;
}
U0 SysCallInit() {
    IDTSet(0x78, SysCallInt, 0x8E);
}