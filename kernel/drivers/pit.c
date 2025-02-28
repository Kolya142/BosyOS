#include <kernel/KDogWatch.h>
#include <misc/driverreg.h>
#include <kernel/KTasks.h>
#include <drivers/pit.h>
#include <lib/MemLib.h>
#include <lib/Time.h>
#include <lib/TTY.h>

volatile U32 PITTime = 0;
volatile U32 BosyTime = 0;
static const U32 days_in_months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

INT_DEF(PITHandler) {
    static volatile U8 timer = 0;
    PITTime++;
    timer++;
    if (timer % 10 == 0) {
        KDogWatchTick();
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
        if (Debugging) {
            VgaCursorDisable();
            U32 c = TTYCursor;
            U8 fg0 = TTYlfg;
            U8 bg0 = TTYlbg;
            TTYCursor = TTYWidth*1 - 5 - 8 - 1;
            PrintF("$!0$*FEIP: %x", regs->eip);
            TTYCursor = TTYWidth*2 - 5 - 8 - 1;
            PrintF("ESP: %x", regs->useresp);
            TTYCursor = TTYWidth*3 - 5 - 8 - 1;
            PrintF("Tim: %x", BosyTime);
            TTYCursor = TTYWidth*4 - 5 - 8 - 1;
            PrintF("mic: %x", TaskTail->regs.eip);
            TTYlbg = bg0;
            TTYlfg = fg0;
            TTYCursor = c;
            VgaCursorEnable();
        }
        if (TaskTail && TaskHead && TaskingIs) {
            MemCpy(&TaskTail->regs, regs, sizeof(INTRegs));
            TaskNext();
            if (TaskTail) {
                MemCpy(regs, &TaskTail->regs, sizeof(INTRegs));
                TaskingIs = True;
            }
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

    DriverReg(0x09ca4b4d, 0x5fa68611, PITDriverHandler);

    POut(0x43, 0x36);
    POut(0x40, div & 0xff);
    POut(0x40, (div>>8) & 0xff);
    POut(0x21, PIn(0x21) & ~1);
    IDTSet(32, PITHandler, 0x08, 0x8E);
}