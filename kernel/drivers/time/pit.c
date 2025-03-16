#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <misc/driverreg.h>
#include <kernel/KTasks.h>
#include <drivers/input/mouse.h>
#include <lib/graphics/Graphics.h>
#include <drivers/time/pit.h>
#include <lib/memory/MemLib.h>
#include <arch/x86/sys/gdt.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>
#include <kws/win.h>

volatile U32 PITTime = 0;
volatile U32 PITTicks = 0;
volatile U32 BosyTime = 0;

extern Bool VRMState;

static inline void regs_copy(INTRegs3 *dest, const INTRegs3 *src) {
    dest->eax      = src->eax;
    dest->ecx      = src->ecx;
    dest->edx      = src->edx;
    dest->ebx      = src->ebx;
    dest->esp      = src->esp;
    dest->ebp      = src->ebp;
    dest->esi      = src->esi;
    dest->edi      = src->edi;
    dest->eflagsp  = src->eflagsp;
    dest->ds       = src->ds;
    dest->es       = src->es;
    dest->gs       = src->gs;
    dest->fs       = src->fs;
    dest->eip      = src->eip;
    dest->cs       = src->cs;
    dest->eflags   = src->eflags;
}

static const U32 days_in_months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

INT_DEF(PITHandler) {
    ++PITTicks;
    PITTime += 1000/60 + 1;

    if (TaskTail && TaskHead) {
        SerialPrintF("bef EIP: %p, Task: %d, ESP: %p, Time: %d", regs->eip, TaskTail->id, TaskTail->regs.useresp, PITTime);
        if (!(TaskTail->flags & TASK_WORKING)) {
            TaskTail->flags |= TASK_WORKING;
            regs_copy(regs, &TaskTail->regs);
        }
        else {
            SerialPrintF("ESP1: %p", TaskTail->regs.useresp);
            regs_copy(&TaskTail->regs, regs);
            SerialPrintF("ESP2: %p", TaskTail->regs.useresp);
            TaskNext();
            SerialPrintF("ESP3: %p", TaskTail->regs.useresp);
            regs_copy(regs, &TaskTail->regs);
            SerialPrintF("ESP4: %p", regs->useresp);
        }
        SerialPrintF("EIP: %p, Task: %d, ESP: %p, Time: %d", regs->eip, TaskTail->id, TaskTail->regs.useresp, PITTime);
        // TTYCurrent = TaskTail->ttyid;
    }
    if (!(PITTicks % 2)) {
        KDogWatchTick();
    }
    
    {
        WindowsUpdate();
        
        if (VRMState) {
            VRMFlush();
            Ptr vrm = VRM;
            VRM = VVRM;
            VRMDrawSprite(vec2(MouseX, MouseY), vec2(6, 8), Black, White, GCursor);
            VRM = vrm;
        }
        if (PITTime % 1000 < 500) {
            for (U32 i = 0; i < 6; ++i) {
                for (U32 j = 0; j < 6; ++j) {
                    U32 x = i + (((TTY*)TTYs.arr)[TTYCurrent].pty->cursor % ((TTY*)TTYs.arr)[TTYCurrent].pty->width)*6;
                    U32 y = j + (((TTY*)TTYs.arr)[TTYCurrent].pty->cursor / ((TTY*)TTYs.arr)[TTYCurrent].pty->width)*6;
                    VVRM[x + y * 320] ^= 15;
                }
            }
        }

        RTCUpdate();
        U32 days = 0;

        for (U32 y = 2025; y < SystemTime.year; y++) {
            days += (y % 4 == 0) ? 366 : 365;
        }
        for (U32 m = 1; m < SystemTime.month; m++) {
            days += days_in_months[m - 1];
            if (m == 2 && (SystemTime.year % 4 == 0)) {
                days++;
            }
        }
        days += SystemTime.day - 1;
        BosyTime = (((days * 24 + SystemTime.hour) * 60) + SystemTime.minute) * 60 + SystemTime.second;
        // SerialPrintF("ESP: %p", regs->useresp);
    }
}

static U0 PITDriverHandler(U32 id, U32 *value) {
    switch (id)
    {
    case 0:
        *value = PITTime;
        break;
    case 1:
        *value = BosyTime;
        break;
    default:
        break;
    }
}

U0 PITInit() {
    U16 div = PIT_FREQ / 60;

    DriverReg(0x09ca4b4d, 0x5fa68611, PITDriverHandler, "pit");

    POut(0x43, 0x36);
    POut(0x40, div & 0xff);
    POut(0x40, (div>>8) & 0xff);
    POut(0x21, PIn(0x21) & ~1);
    IDTSet(32, PITHandler, 0x08, 0x8E);
}