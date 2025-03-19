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
#include <kws/input.h>
#include <kws/win.h>

static inline Bool is_userspace(U32 addr) {
    return 1 || addr >= 0x0C000000;
}

U0 SCDriver(INTRegs3 *regs) {
    DriverCall(regs->ebx, regs->ecx, regs->edx, (U32*)regs->ebx);
}
U0 SCExit(INTRegs3 *regs) {
    TaskClose();
}
U0 SCFork(INTRegs3 *regs) {
    regs->eax = TFork();
}

U0 SCRead(INTRegs3 *regs) {
    U32 fd = regs->ebx;
    U32 buf = regs->ecx;
    U32 count = regs->edx;
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
    U32 fd = regs->ebx;
    U32 buf = regs->ecx;
    U32 count = regs->edx;
    if (fd <= 2) {
        regs->eax = TTYWrite(TaskTail->ttyid, fd, (Ptr)buf, count);
    }
    else if (fd >= 3) {
        regs->eax = VFSWriteV(fd, (Ptr)buf, count);
    }
    else {
        regs->eax = 0;
    }
}

U0 SCOpen(INTRegs3 *regs) {
    regs->eax = VFSOpen((String)regs->ebx);
}
U0 SCClose(INTRegs3 *regs) {
    VFSClose(regs->ebx);
}

U0 SCMAlloc(INTRegs3 *regs) {
    regs->eax = (U32)MAlloc(regs->ebx);
}
U0 SCFree(INTRegs3 *regs) {
    MFree((Ptr)regs->ebx);
}

U0 SCExecA(INTRegs3 *regs) {
    regs->eax = TaskNew(regs->ebx, 0x23, 0x1B);
}

U0 SCReadDir(INTRegs3 *regs) {
    VFSReadDir((String)regs->ebx, (Ptr)regs->ecx);
}

U0 SCTime(INTRegs3 *regs) {
    if (!is_userspace(regs->ebx)) {
        regs->eax = 1;
        return;
    }
    
    U32 *t = (U32*)regs->ebx;
    *t = BosyTime + BOSY_EPOCH;
    regs->eax = 0;
}

U0 SCLSeek(INTRegs3 *regs) {
    U32 fd = regs->ebx;
    U32 off = regs->ecx;
    U32 whence = regs->edx;
    if (fd == 1) {
        switch (whence) {
            case 0: // SET
                ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor = off;
                break;
            case 1: // CUR
                ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor += off;
                break;
            case 2: // END
                TTY *t = &((TTY*)TTYs.arr)[TTYCurrent];
                t->pty->cursor = t->pty->width * t->pty->height + off;
                break;
        }
    }
    else {
        VFSFD *file = (VFSFD*)fd;
        switch (whence) {
            case 0: // SET
                file->head = off;
                break;
            case 1: // CUR
                file->head += off;
                break;
            case 2: // END
                break;
        }
    }
    regs->eax = 0;
}

U0 SCGetPid(INTRegs3 *regs) {
    regs->eax = TaskTail->id;
}
#define TIOWINSZ 54
#define TIOGTCUR 55
#define TIOSTTTY 91
#define TIOGTTTY 92
#define TIONWTTY 63
#define TIONWPTY 62

#define WIOCREAT 10
#define WIOKILL  11
#define WIOUPDFN 20
U0 SCIOCTL(INTRegs3 *regs) {
    U32 fd = regs->ebx;
    U32 req = regs->ecx;

    if (fd == 1) {
        if (req == TIOWINSZ) {
            if (!regs->edx || !regs->esi) {
                regs->eax = 2;
                return;
            }
            if (!is_userspace(regs->edx) || !is_userspace(regs->esi)) {
                regs->eax = 2;
                return;
            }
            *((U32*)regs->edx) = ((TTY*)TTYs.arr)[TTYCurrent].pty->width;
            *((U32*)regs->esi) = ((TTY*)TTYs.arr)[TTYCurrent].pty->height;
            regs->eax = 0;
        }
        else if (req == TIOGTCUR) {
            if (!regs->edx || !regs->esi) {
                regs->eax = 2;
                return;
            }
            if (!is_userspace(regs->edx) || !is_userspace(regs->esi)) {
                regs->eax = 2;
                return;
            }
            *((U32*)regs->edx) = ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor;
            regs->eax = 0;
        }
        else if (req == TIOSTTTY) {
            if (regs->edx >= TTYs.count) {
                regs->eax = 2;
            }
            TaskTail->ttyid = regs->edx;
            regs->eax = 0;
        }
        else if (req == TIOGTTTY) {
            *((U32*)regs->edx) = TaskTail->ttyid;
            regs->eax = 0;
        }
        else if (req == TIONWTTY) {
            if (!is_userspace(regs->edi)) {
                regs->eax = 2;
                return;
            }
            if (regs->edx >= PTYs.count) {
                regs->eax = 2;
            }
            *((U32*)regs->esi) = TTYNew(((TTY*)TTYs.arr)[TaskTail->ttyid].flush, regs->edx);
            regs->eax = 0;
        }
        else if (req == TIONWPTY) {
            if (!is_userspace(regs->edi)) {
                regs->eax = 2;
                return;
            }
            *((U32*)regs->edi) = PTYNew(2048, regs->edx, regs->esi);
            regs->eax = 0;
        }
    }
    else if (fd == 3) {
        switch (req) {
            case WIOCREAT: {
                Win win = WinMake(50, 50, regs->edx, regs->esi, (String)regs->edi, regs->ebp);
                regs->eax = WinSpawn(&win);
            } break;
            case WIOUPDFN: {
                windows[regs->edx].update = (Ptr)regs->esi;
            } break;
            case WIOKILL : {
                windows[regs->edx].w = 0;
            } break;
        }
    }
    else {
        regs->eax = 1;
    }
}

U0 SCStat(INTRegs3 *regs) {
    if (!is_userspace(regs->ebx) || !is_userspace(regs->ecx)) {
        return;
    }
    VFSLStat((String)regs->ebx, (VFSStat*)regs->ecx);
}

U0 SCSigRet(INTRegs3 *regs) {
    TaskTail->regs.eip = TaskTail->saved_eip;
    TaskTail->regs.esp = TaskTail->saved_esp;
}

U0 SCClockGetTime(INTRegs3 *regs) {
    struct time_spec {
        U32 sec;
        U32 nsec;
    };
    struct time_spec *time = (struct time_spec*)regs->ecx;

    time->sec = BosyTime + BOSY_EPOCH;

    time->nsec = (PITTime % 1000) * 1000000;
    regs->eax = 0;
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
    SysCallSet(SCReadDir, 12);
    SysCallSet(SCTime, 13);
    SysCallSet(SCLSeek, 19);
    SysCallSet(SCGetPid, 20);
    SysCallSet(SCIOCTL, 54);
    SysCallSet(SCStat, 106);
    SysCallSet(SCClockGetTime, 265);
}