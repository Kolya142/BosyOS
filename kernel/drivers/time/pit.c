#include <kernel/KDogWatch.h>
#include <misc/driverreg.h>
#include <kernel/KTasks.h>
#include <drivers/input/mouse.h>
#include <lib/graphics/Graphics.h>
#include <drivers/time/pit.h>
#include <lib/memory/MemLib.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>

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
            TTerm.render();
            VgaCursorDisable();
            U32 c = TTYCursor;
            TTYCursor = TTerm.width*2-13;
            PrintF("EIP: %x", regs->eip);
            TTYCursor = TTerm.width*3-13;
            PrintF("ESP: %x", regs->useresp);
            TTYCursor = TTerm.width*4-13;
            PrintF("Tim: %x", BosyTime);
            TTYCursor = TTerm.width*5-13;
            PrintF("Tsk: %x", TaskTail->id);
            TTYCursor = TTerm.width*6-13;
            PrintF("CS: %x", regs->cs);
            TTYCursor = TTerm.width*7-13;
            PrintF("DS: %x", regs->ds);
            TTYCursor = TTerm.width*8-13;
            PrintF("MS: %x", PITTime);
            TTerm.render();
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