#include <kernel/KDogWatch.h>
#include <misc/driverreg.h>
#include <kernel/KTasks.h>
#include <drivers/mouse.h>
#include <lib/Graphics.h>
#include <drivers/pit.h>
#include <lib/MemLib.h>
#include <lib/Time.h>
#include <lib/TTY.h>

volatile U32 PITTime = 0;
volatile U32 BosyTime = 0;
static const U32 days_in_months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

extern Bool VRMState;

INT_DEF(PITHandler) {
    static volatile U8 timer = 0;
    PITTime++;
    timer++;
    if (timer % 10 == 0) {
        KDogWatchTick();
        if (VRMState) {
            MouseUpdate();
            VRMFlush();
            Ptr vrm = VRM;
            VRM = VVRM;
            VRMDrawSprite(vec2(MouseX, MouseY), vec2(6, 8), Black, White, GCursor);
            VRM = vrm;
        }
        if (PITTime % 1000 < 500) {
            for (U32 i = 0; i < 6; ++i) {
                for (U32 j = 0; j < 6; ++j) {
                    U32 x = i + (TTYCursor % TTYWidth)*6 + TTYCursorX;
                    U32 y = j + (TTYCursor / TTYWidth)*6 + TTYCursorY;
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
        if (TaskTail && TaskHead) {
            // PrintF("Bef RGS: %x\n", regs->eax+regs->ebx+regs->ecx+regs->edx+regs->esi+regs->edi);
            if (TaskTail->flags & TASK_WORKING)
                MemCpy(&TaskTail->regs, regs, sizeof(INTRegs));
            else
                TaskTail->flags |= TASK_WORKING;
            TaskNext();
            MemCpy(regs, &TaskTail->regs, sizeof(INTRegs));
            // PrintF("Aft RGS: %x\n", regs->eax+regs->ebx+regs->ecx+regs->edx+regs->esi+regs->edi);
        }
        if (Debugging) {
            VgaCursorDisable();
            U32 c = TTYCursor;
            U8 fg0 = TTYlfg;
            U8 bg0 = TTYlbg;
            TTYCursor = TTYWidth*1-13;
            PrintF("EIP: %x", regs->eip);
            TTYCursor = TTYWidth*2-13;
            PrintF("ESP: %x", regs->useresp);
            TTYCursor = TTYWidth*3-13;
            PrintF("Tim: %x", BosyTime);
            TTYCursor = TTYWidth*4-13;
            PrintF("Tsk: %x", TaskTail->id);
            TTYCursor = TTYWidth*5-13;
            PrintF("CS: %x", regs->cs);
            TTYCursor = TTYWidth*6-13;
            PrintF("DS: %x", regs->ds);
            TTYCursor = TTYWidth*7-13;
            PrintF("MS: %x", PITTime);
            TTYlbg = bg0;
            TTYlfg = fg0;
            TTYCursor = c;
        }   
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