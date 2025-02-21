#include <kernel/KDogWatch.h>
#include <drivers/keyboard.h>
#include <kernel/KPanic.h>
#include <misc/syscall.h>
#include <arch/getreg.h>
#include <lib/MemLib.h>
#include <arch/beep.h>
#include <lib/TTY.h>
#include <arch/cpu.h>
#include <arch/io.h>

U0 SCPrint() {
    U32 addr = RegGet(REG_ESI);
    TTYUPrint((Ptr)addr);
}
U0 SCPrintH() {
    U32 data = RegGet(REG_ESI);
    TTYUPrintHex(data);
}
U0 SCBeep() {
    U16 data = RegGet(REG_ESI);
    Beep(data);
}
U0 SCSleep() {
    U16 data = RegGet(REG_ESI);
    SleepM(data);
}
U0 SCKey() {
    asmV("movb %0, %%al" :: "r"(KBState.Key));
}
U0 SCPutC() {
    U8 c = RegGet(REG_ESI);
    TTYUPrintC(c);
}

U0 SysCallSetup() {
    SysCallSet(SCPrint,  1);
    SysCallSet(SCPrintH, 2);
    SysCallSet(SCBeep,   3);
    SysCallSet(SCSleep,  4);
    SysCallSet(SCKey,    5);
    SysCallSet(SCPutC,   6);
}