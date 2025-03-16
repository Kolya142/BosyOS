#include <kernel/KDogWatch.h>
#include <drivers/input/keyboard.h>
#include <drivers/serial/serial.h>
#include <misc/driverreg.h>
#include <kernel/KPanic.h>
#include <kernel/KTasks.h>
#include <lib/sys/syscall.h>
#include <arch/x86/cpu/getreg.h>
#include <lib/formats/bsfexe.h>
#include <drivers/misc/random.h>
#include <lib/memory/MemLib.h>
#include <drivers/sys/beep.h>
#include <arch/x86/sys/gdt.h>
#include <arch/x86/sys/idt.h>
#include <arch/x86/cpu/cpu.h>
#include <arch/x86/sys/sys.h>
#include <lib/IO/TTY.h>
#include <arch/x86/cpu/io.h>

U0 SCDriver(INTRegs3 *regs) {
    DriverCall(regs->esi, regs->edi, regs->ebx, (U32*)regs->edx);
}
U0 SCExit(INTRegs3 *regs) {
    TaskClose();
}
U0 SCFork(INTRegs3 *regs) {
    regs->eax = TFork();
}

U0 SCRead(INTRegs3 *regs) {
    U32 fd = regs->esi;
    U32 buf = regs->edi;
    U32 count = regs->ebx;
    if (fd <= 2) {
        regs->eax = TTYRead(TaskTail->ttyid, fd, (Ptr)buf, count);
    }
    else if (fd >= 3) {
        regs->eax = VFSReadV(fd, (Ptr)buf, count);
    }
    else {
        regs->eax = 0;
    }
}

U0 SCWrite(INTRegs3 *regs) {
    U32 fd = regs->esi;
    U32 buf = regs->edi;
    U32 count = regs->ebx;
    if (fd <= 2) {
        regs->eax = TTYWrite(TaskTail->ttyid, fd, (Ptr)buf, count);
        TTYFlush(TaskTail->ttyid);
    }
    else if (fd >= 3) {
        regs->eax = VFSWriteV(fd, (Ptr)buf, count);
    }
    else {
        regs->eax = 0;
    }
}

U0 SCOpen(INTRegs3 *regs) {
    regs->eax = VFSOpen((String)regs->esi);
}
U0 SCClose(INTRegs3 *regs) {
    VFSClose(regs->esi);
}

U0 SCMAlloc(INTRegs3 *regs) {
    regs->eax = (U32)MAlloc(regs->esi);
}
U0 SCFree(INTRegs3 *regs) {
    MFree((Ptr)regs->esi);
}

U0 SCExecA(INTRegs3 *regs) {
    regs->eax = TaskNew(regs->esi, 0x23, 0x1B);
}

U0 SCTime(INTRegs3 *regs) {
    regs->eax = BosyTime + BOSY_EPOCH;
}

U0 SCGetPid(INTRegs3 *regs) {
    regs->eax = TaskTail->id;
}

U0 SCIOCTL(INTRegs3 *regs) {
    U32 fd = regs->esi;
    U32 req = regs->edi;

    if (req == 0 && fd == 1) {
        if (!regs->ebx || !regs->edx) {
            regs->eax = 2;
            return;
        }
        *((U32*)regs->ebx) = ((TTY*)TTYs.arr)[TTYCurrent].pty->width;
        *((U32*)regs->edx) = ((TTY*)TTYs.arr)[TTYCurrent].pty->height;
        regs->eax = 0;
    }
    else {
        regs->eax = 1;
    }
}

U0 SCStat(INTRegs3 *regs) {
    VFSLStat((String)regs->esi, (VFSStat*)regs->edi);
}

U0 SysCallSetup() {
    SysCallSet(SCDriver, 0);
    SysCallSet(SCExit, 1);
    SysCallSet(SCFork, 2);
    SysCallSet(SCRead, 3);
    SysCallSet(SCWrite, 4);
    SysCallSet(SCOpen, 5);
    SysCallSet(SCClose, 6);
    SysCallSet(SCMAlloc, 7);
    SysCallSet(SCFree, 8);
    SysCallSet(SCExecA, 11);
    SysCallSet(SCTime, 13);
    SysCallSet(SCGetPid, 20);
    SysCallSet(SCIOCTL, 54);
    SysCallSet(SCStat, 106);
}