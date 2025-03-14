#include <kernel/KDogWatch.h>
#include <misc/driverreg.h>
#include <kernel/KTasks.h>
#include <drivers/input/mouse.h>
#include <lib/graphics/Graphics.h>
#include <drivers/time/pit.h>
#include <lib/memory/MemLib.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>
#include <kws/win.h>

volatile U32 PITTime = 0;
volatile U32 BosyTime = 0;

extern Bool VRMState;

static const U32 days_in_months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

INT_DEF(PITHandler) {
    PITTime++;
    if (!(PITTime % 64)) {
        KDogWatchTick();

        if (TaskTail && TaskHead) {
            if (!(TaskTail->flags & TASK_WORKING)) {
                TaskTail->flags |= TASK_WORKING;
                MemCpy(regs, &TaskTail->regs, sizeof(INTRegs));
            }
            else {
                MemCpy(&TaskTail->regs, regs, sizeof(INTRegs));
                // asmv("fxsave (%0)" :: "r"(TaskTail->fpu)); // TODO: save fpu
                TaskNext();
                // asmV("fxrstor (%0)" :: "r"(TaskTail->fpu));
                MemCpy(regs, &TaskTail->regs, sizeof(INTRegs));
            }
            // SerialPrintF("EIP: %p, Task: %d, ESP: %p, Time: %d", regs->eip, TaskTail->id, regs->useresp, PITTime);
        }


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
                    U32 x = i + (TTYCursor % TTerm.width)*6;
                    U32 y = j + (TTYCursor / TTerm.width)*6;
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
    U16 div = PIT_FREQ / 1900;

    DriverReg(0x09ca4b4d, 0x5fa68611, PITDriverHandler, "pit");

    POut(0x43, 0x36);
    POut(0x40, div & 0xff);
    POut(0x40, (div>>8) & 0xff);
    POut(0x21, PIn(0x21) & ~1);
    IDTSet(32, PITHandler, 0x08, 0x8E);
}