#include <misc/driverreg.h>
#include <kernel/KTasks.h>
#include <drivers/pit.h>
#include <arch/beep.h>

U0 Beep(U16 dur) {
    POut(0x61, PIn(0x61) | 3);
    TSleep(dur);
    POut(0x61, PIn(0x61) & ~3);
}
U0 BeepHz(U16 freq, U16 dur) {
    if (freq < 10 || freq > 20000) return;
    U32 Div = 1193180 / (U32)freq;
    POut(0x43, 0xB6);
    POut(0x42, (U8)(Div));
    POut(0x42, (U8)(Div >> 8));

    U8 tmp = PIn(0x61);
    if (!(tmp & 3)) {
        POut(0x61, tmp | 3);
    }
    TSleep(dur);
    POut(0x61, PIn(0x61) & ~3);
    PITInit(); // PIT back
}

U0 BeepSPC(U8 tone, U16 dur) {
    static const U16 NoteTable[128] = {
        16, 17, 18, 19, 21, 22, 23, 25, 26, 28, 29, 31,                         // C0 - B0
        33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,                         // C1 - B1
        65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123,                     // C2 - B2
        131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,             // C3 - B3
        262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,             // C4 - B4
        523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,             // C5 - B5
        1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, // C6 - B6
        2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, // C7 - B7
        4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902  // C8 - B8
    };
    if (tone >= 128) return;
    BeepHz(NoteTable[tone], dur);
}

static U0 BeepDriverHandler(U32 id, U32 *value) {
    switch (id)
    {
    case 0:
        Beep(*value & 0xFFFF);
        break;
    case 1:
        BeepHz((*value >> 16) & 0xFFFF, *value & 0xFFFF);
        break;
    case 2:
        BeepSPC((*value >> 16) & 0xFFFF, *value & 0xFFFF);
        break;
    default:
        break;
    }
}

U0 BeepInit() {
    DriverReg(0x46ef3f2c, 0x27e134cd, BeepDriverHandler);
}