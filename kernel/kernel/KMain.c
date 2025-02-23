// Kernel
#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>

// Drivers
#include <drivers/keyboard.h>
#include <drivers/vga.h>
#include <drivers/pit.h>

// Miscellaneous
#include <misc/syscall.h>
#include <misc/wordgen.h>
#include <misc/bsf.h>

// Libraries
#include <lib/KeyboardLib.h>
#include <lib/String.h>
#include <lib/Random.h>
#include <lib/MemLib.h>
#include <lib/Time.h>
#include <lib/DAT.h>
#include <lib/TTY.h>
#include <lib/BosZ.h>

// Arch/Cpu Functions
#include <arch/getreg.h>
#include <arch/beep.h>
#include <arch/cpu.h>
#include <arch/gdt.h>
#include <arch/sys.h>
#include <arch/pic.h>

U0 programtest();
U0 mainloop();

__attribute__((naked)) U0 KernelMain() {
    if (*(Byte*)(0x7C00 + 510) == 0x55) {
        for (U32 i = 0x7C00; i < 0x8000; i++) { // Clear bootloader
            *(Byte*)i = 0;
        }
    }
    TTYClear();
    KDogWatchInit();
    GDTInit();
    KDogWatchLog("GDT Initialized", False);
    IDTInit();
    PICMap();
    PITInit();
    KBInit();
    VgaBlinkingSet(False);
    VgaCursorDisable();
    VgaCursorEnable();
    VgaCursorSet(0, 0);
    SysCallInit();
    SysCallSetup();
    DATInit();
    TTYUPrint(
        "\n"
        "$!EBosyOS control codes$!F: $*F$!0See *mezex.txt*$*0$!F\n");
    programtest();
    KDogWatchPStart(0, "Main loop");
    mainloop();
    KDogWatchPEnd(0);
    PowerOff();
}

U0 CHelp() {
    TTYUPrint(
        "$!5Commands$!F:\n"
        "$!Ahelp cls hlt gen$!F\n"
        "$!Btime echo wsave rand$!F\n"
        "$!Creboot poweroff apple$!F\n"
        "$!Esnake text sound wget$!F\n"
        "$!3words triangle bf hz$!F\n"
        "$!5Features$!F:\n"
        "All Commands: esc-exit\n"
        "$!Asound$!F - press $!Balt$!F\n"
        "$!Awsave$!F/$!Awget$!F uses disk\n"
        "use $!BWASD$!F in $!Atext$!F to move cursor"
    );
}
U0 CCls() {
    TTYClear();
    TTYCursor = 0;
}
U0 CWords() {
    TTYClear();
    TTYCursor = 0;
    while (TTYCursor < 80*20) {
        WordGen();
    }
    TTYUPrintC('\n');
}
U0 CTriangle() {
    TTYClear();
    TTYCursor = 0;
    Bool state[25] = {0};
    state[0] = True;
    for (;!KBState.keys['\x1b'];) {
        for (U32 i = 25; i > 0; --i) {
            if (state[i-1]) {
                state[i] = !state[i];
            }
        }
        U32 count = 0;
        for (U32 j = 0; j < 25; ++j) {
            count += state[j];
            VgaPSet(0, 25 - j, state[j] ? 0xDB : ' ', 0x0F);
            for (U32 i = 80; i > 0; --i) {
                VgaPSet(i, 25 - j, vga[i - 1 + (25 - j) * 80] & 0xff, 0x0F);
            }
        }
        BeepSPC(count + 30, 30);
        BeepSPC(count + 50, 10);
        BeepSPC(count + 70, 5);            
        SleepM(20);
        KDogWatchPTick(0);
    }
}
U0 CTime() {
    TTYUPrint("$!BPIT$!F:    "); TTYUPrintHex(PITTime); TTYUPrintC('\n');
    RTCUpdate();
    TTYUPrint("$!BYear$!F:   "); TTYUPrintDec(SystemTime.year); TTYUPrintC('\n');
    TTYUPrint("$!BMonth$!F:  "); TTYUPrintDec(SystemTime.month); TTYUPrintC('\n');
    TTYUPrint("$!BDay$!F:    "); TTYUPrintDec(SystemTime.day); TTYUPrintC('\n');
    TTYUPrint("$!BHour$!F:   "); TTYUPrintDec(SystemTime.hour); TTYUPrintC('\n');
    TTYUPrint("$!BMinute$!F: "); TTYUPrintDec(SystemTime.minute); TTYUPrintC('\n');
    TTYUPrint("$!BSecond$!F: "); TTYUPrintDec(SystemTime.second); TTYUPrintC('\n');
}
U0 CRand() {
    Bool lk = False;
    I16 note = 60;
    KDogWatchPEnd(0);
    U32 c = TTYCursor;
    U32 v = 50542;
    for (U32 i = 0; !KBState.keys['\x1b']; ++i) {
        note += ((I32)RandomU() % 6) - 3;
        if (note < 20) note = 20;
        if (note > 150) note = 150;
        U16 dur = RandomU() % 150 + 50;
        BeepSPC(note, dur);
        vga[(v >> 16) % 1999] = v & 0xffff;
        v = calculate_shash(v * 50239 + note ^ dur);
        if (RandomU() % 5 <= 2) SleepM(RandomU() % 200 + 100);
        else if (RandomU() % 4 <= 1) {
            BeepSPC(note + 4, dur);
            BeepSPC(note + 7, dur);
        }
        else if (RandomU() % 5 == 1) {
            BeepSPC(note + 4, dur);
            BeepSPC(note + 7, dur);
            BeepSPC(note + 11, dur);
            BeepSPC(note + 13, dur);
        }
        WordGen();
        if (!(i % 5))
            TTYCursor = c;
        SleepM(1000/120); // 120gh
    }
    KDogWatchPStart(0, "Main loop");
}
U0 CGen() {
    Bool lk = False;
    I16 note = 60;
    for (U32 i = 0; !KBState.keys['\x1b']; ++i) {
        for (U32 x1 = 0; x1 < 80; ++x1) {
            for (U32 y1 = 0; y1 < 25; ++y1) {
                vga[x1 + y1 * 80] = (calculate_shash((x1+i)^y1)) & 15 << 12 | ' ';
            }
        }
        note += ((I32)RandomU() % 30) - 15;
        if (note < 20) note = 20;
        if (note > 150) note = 150;
        BeepSPC(note, 30);
        KDogWatchPTick(0);
        SleepM(1000/30);
    }
}
U0 CText() {
    CCls();
    if (DATFind(0x5234) == 0xFFFF) {
        DATAlloc(0x5234);
    }
    else {
        U8 buf[20*18] = {0};
        DATRead(0x5234, buf, 0, 20*18);
        U32 i = 0;
        for (U32 x = 2; x < 30; ++x) {
            for (U32 y = 2; y < 20; ++y) {
                vga[x + y*80] = buf[i] ? (buf[i] | 0x0F00) : 0x0F20;
                ++i;
            }
        }
    }
    TTYUPrint("Press p");
    Bool text = False;
    Bool lk[256] = {0};
    KDogWatchPEnd(0);
    VgaCursorEnable();
    U32 x = TTYCursor % 80;
    U32 y = TTYCursor / 80;
    for (;;) {
        TTYCursor = 0;
        VgaCursorSet(x, y);
        while (!KBState.keys['p']);
        TTYUPrint("Enter text: ");
        Char cmd[30] = {0};
        KBRead(cmd, 30);
        VgaCursorSet(x, y);
        if (cmd[0] == ':') {
            for (U32 i = 0; cmd[i]; ++i) {
                switch (cmd[i]) {
                    case 'q': {
                        U8 buf[20*18] = {0};
                        U32 i = 0;
                        for (U32 x = 2; x < 30; ++x) {
                            for (U32 y = 2; y < 20; ++y) {
                                buf[i] = vga[x + y*80] & 0xff;
                                ++i;
                            }
                        }
                        KDogWatchPPlay(0);
                        DATWrite(0x5234, buf, 0, 20*18);
                        VgaCursorDisable();
                        return;
                    } break;
                    case 'l': 
                    --x;
                    break;
                    case 'D': 
                    vga[x--] = 0;
                    break;
                    case 'r': 
                    ++x;
                    break;
                    case 'u': 
                    --y;
                    break;
                    case 'd': 
                    ++y;
                    break;
                    case 'c': 
                    CCls();
                    break;
                    case 'n': {
                        x = 2;
                        ++y;
                    } break;
                }
            }
        }
        else {
            if (x < 2) x = 2;
            if (x > 29) x = 29;
            if (y < 2) y = 2;
            if (y > 19) y = 19;
            TTYCursor = x + y * 80;
            TTYUPrint(cmd);
        }
        if (x < 2) x = 2;
        if (x > 29) x = 29;
        if (y < 2) y = 2;
        if (y > 19) y = 19;
        TTYCursor = x + y * 80;
        VgaCursorSet(TTYCursor, 0);
    }
}

U0 CSound() {
    static const U8 KeyToNote[128] = {
        40, 42, 44, 45, 47, 49, 51, 52, 54, 56, 57, 59,
        60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79,
        81, 83, 85, 87, 89, 91, 93, 95,
        40, 42, 44, 45, 47, 49, 51, 52, 54, 56, 57, 59,
        60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79,
        81, 83, 85, 87, 89, 91, 93, 95,
        40, 42, 44, 45, 47, 49, 51, 52, 54, 56, 57, 59,
        60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79,
        81, 83, 85, 87, 89, 91, 93, 95,
        40, 42, 44, 45, 47, 49, 51, 52, 54, 56, 57, 59,
        60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79,
        81, 83, 85, 87, 89, 91, 93, 95
    };
    
    U8 Record[510];
    MemSet(Record, 0, 510);
    U32 i = 0;

    for (;;) {
        if (KBState.keys['\x1b']) {
            break;
        }
        if (KBState.keys[ASCIIPAlt]) {
            for (U32 j = 0; Record[j]; ++j) {
                BeepSPC(Record[j], 1);
                KDogWatchPTick(0);
            }
        }
        TTYUPrintHex(i>>1);
        if (!(KBState.SC & 0x80)) {
            U8 note = KeyToNote[KBState.SC%sizeof(KeyToNote)];
            TTYUPrintHex(note);
            BeepSPC(note, 3);
            Record[i>>1] = note;
            i = (i+1) % (sizeof(Record) << 2);
        }
        TTYCursor -= TTYCursor % 80;
        KDogWatchPTick(0);
        SleepM(3);
    }
}
U0 CSnake() {
    KDogWatchPEnd(0);
    U32 tail[5] = {0};
    tail[0] = 80 * 10 + 39;
    tail[1] = 80 * 10 + 38;
    tail[2] = 80 * 10 + 37;
    tail[3] = 80 * 10 + 36;
    tail[4] = 80 * 10 + 35;
    U32 snake = 80 * 10 + 40;
    U32 apple = RandomU() % 1300 + 500;
    U32 score = 0;
    U8 dir = 4;
    for (U32 i = 0; i < 2000; ++i) {
        vga[i] = 0xF7B0;
    }
    Bool game = True;
    while (game) {
        if (KBState.keys['w']) {
            if (dir != 1)
                BeepSPC(54, 30);
            dir = 1;
        }
        else if (KBState.keys['a']) {
            if (dir != 2)
                BeepSPC(54, 30);
            dir = 2;
        }
        else if (KBState.keys['s']) {
            if (dir != 3)
                BeepSPC(54, 30);
            dir = 3;
        }
        else if (KBState.keys['d']) {
            if (dir != 4)
                BeepSPC(54, 30);
            dir = 4;
        }

        for (U8 i = 4; i >= 1; --i) {
            vga[tail[i]] = 0xF7B0;
            tail[i] = tail[i-1];
        }
        tail[0] = snake;

        if (snake == apple) {
            score++;
            vga[apple] = 0xF02E;
            apple = RandomU() % 1300 + 500;
            BeepSPC(50, 70);
            BeepSPC(30, 40);
            BeepSPC(50, 70);
        }

        if (dir == 1      && ((snake - 80) < 55555)) {
            snake -= 80;
        }
        else if (dir == 2 && (snake - 1) % 80 != 79) {
            snake -= 1;
        }
        else if (dir == 3 && ((snake + 80) < 2000))  {
            snake += 80;
        }
        else if (dir == 4 && ((snake +1) % 80 != 0)) {
            snake += 1;
        }

        for (U8 i = 0; i < 5; ++i) {
            TTYCursor = tail[i];
            TTYRawPrint('#', Black, White);
            // if (tail[i] == snake) {
            //     for (U32 i = 0; i < 2000; ++i) {
            //         vga[i] ^= 0xFF00;
            //     }
            //     TTYCursor = 0;
            //     TTYUPrint("Game over!\n");
            //     game = False;
            //     break;
            // }
        }
        if (KBState.keys['\x1b']) {
            game = False;
        }
        TTYCursor = apple;
        TTYRawPrint(' ', Red, Red);
        TTYCursor = 80 / 2 - 4 / 2;
        TTYUPrintHex(score);
        BeepSPC(45, 15);
        BeepSPC(25, 10);
        BeepSPC(30, 20);
        SleepM(100);
    }
    KDogWatchPStart(0, "Main loop");
}
U0 BFInterpret(const String code) {
    U32 stack[32] = {0};
    U8  mem[3355] = {0};
    U32 stacki = 0;
    U32 memi = 0;
    for (U32 i = 0; code[i]; ++i) {
        switch (code[i]) {
            case '+':
                ++mem[memi];
                break;
            case '-':
                --mem[memi];
                break;
            case '<':
                --memi;
                break;
            case '>':
                ++memi;
                break;
            case '[': {
                U32 j = 1;
                if (!mem[memi]) {
                    while (j) {
                        ++i;
                        if (!code[i]) break;
                        if (code[i] == '[') ++j;
                        if (code[i] == ']') --j;
                    }
                }
                else {
                    stack[stacki++] = i;
                }
                }
                break;
            case ']':
                if (mem[memi]) {
                    i = stack[stacki--] - 1;
                }
                else {
                    --stacki;
                }
                break;
            case '.':
                TTYUPrintC(mem[memi]);
                break;
            case '/':
                TTYUPrintDec(mem[memi]);
                break;
        }
    }
}
U0 CHz() {
    KDogWatchPEnd(0);
    for (;;) {
        for (U32 s = 1; s < 200; ++s) {
            for (U32 o = 0; o < 50; ++o) {
                static const U16 tones[12] = {
                    100, 106, 112, 119, 126, 133, 141, 150, 159, 169, 179, 190
                };
                U32 f = 30 + tones[s%12] - o;
                BeepHz(f, f / 6);
                TTYUPrintHex(f);
                TTYCursor -= 8;
                if (KBState.keys['\x1b']) 
                    break;
            }
            if (KBState.keys['\x1b']) 
                break;
        }
        if (KBState.keys['\x1b']) 
            break;
    }
    KDogWatchPPlay(0);
}

U0 termrun(const String cmd) {
    if (!StrCmp(cmd, "help")) {
        CHelp();
    }
    else if (!StrCmp(cmd, "cls")) {
        CCls();
    }
    else if (!StrCmp(cmd, "words")) {
        CWords();
    }
    else if (!StrCmp(cmd, "triangle")) {
        CTriangle();
    }
    else if (!StrCmp(cmd, "hlt")) {
        asmv("cli");
        CpuHalt();
    }
    else if (!StrCmp(cmd, "hz")) {
        CHz();
    }
    else if (!StrCmp(cmd, "time")) {
        CTime();
    }
    else if (StrStartsWith(cmd, "echo ")) {
        TTYUPrint(cmd+4);
    }
    else if (!StrCmp(cmd, "echo")) {
        TTYUPrint(".");
    }
    else if (!StrCmp(cmd, "rand")) {
        CRand();
    }
    else if (!StrCmp(cmd, "gen")) {
        CGen();
    }
    else if (!StrCmp(cmd, "reboot")) {
        PowerReboot();
    }
    else if (!StrCmp(cmd, "poweroff")) {
        KDogWatchPEnd(0);
        PowerOff();
    }
    else if (!StrCmp(cmd, "text")) {
        CText();
    }
    else if (!StrCmp(cmd, "sound")) {
        CSound();
    }
    else if (!StrCmp(cmd, "snake")) {
        CSnake();
    }
    else if (!StrCmp(cmd, "bf")) {
        CCls();
        TTYUPrint("$!DRun existing code(Y/N)$!F?");
        KDogWatchPEnd(0);
        {
            Char buf[1];
            KBRead(buf, 2);
            TTYUPrintC('\n');
            if (buf[0] == 'y' || buf[0] == 'Y') {
                Char ubuf[500];
                if (DATFind(0x6266) == 0xFFFF) {
                    TTYUPrint("$!CNo code found$!F.\n");
                    return;
                }
                DATRead(0x6266, ubuf, 0, 500);
                if (ubuf[0] == '\0') {
                    TTYUPrint("$!CEmple code$!F.\n");
                    return;
                }
                TTYUPrint("$!ACode$!F:");
                TTYUPrint(ubuf);
                TTYUPrint("\n\n");
                BFInterpret(ubuf);
            }
            else {
                TTYUPrint("$!DEnter bf code$!F:");
                Char ubuf[500];
                KBRead(ubuf, 500);
                DATAlloc(0x6266);
                DATWrite(0x6266, ubuf, 0, 500);
                BFInterpret(ubuf);
                TTYUPrint("\n$!ACode saved...$!F");
            }
        }
        KDogWatchPPlay(0);
    }
    else if (!StrCmp(cmd, "wsave")) {
        DATAlloc(0x1254);
        TTYUPrint("$!BPrevius word$!F: ");
        U8 buf[100] = {0};
        DATRead(0x1254, buf, 0, 100);
        TTYUPrint(buf);
        TTYUPrint("\n$!CNew word$!F: ");
        KBRead(buf, 100);
        DATWrite(0x1254, buf, 0, 100);
    }
    else if (!StrCmp(cmd, "wget")) {
        TTYUPrint("$!Bword$!F: ");
        U8 buf[100] = {0};
        DATRead(0x1254, buf, 0, 100);
        TTYUPrint(buf);
    }
    else {
        TTYUPrint("Unk ");
        TTYUPrint(cmd);
    }
}

U0 mainloop() {
    Char buffer[50] = {0};
    for (;;) {
        KDogWatchPEnd(0);
        TTYUPrint("$!A\\$ $!F");
        KBRead(buffer, 50);
        KDogWatchPPlay(0);
        TTYUPrintC('\n');
        termrun(buffer);
        TTYUPrintC('\n');
        MemSet(buffer, 0, 50);
    }
}
U0 programtest()
{
    BsfApp bapp = BsfFromBytes((Ptr)0x8400);
    // UserSegment = (Segment) {
    //     .addr = bapp.data,
    //     .length = bapp.header.CodeS,
    // };
    BsfExec(&bapp);
    // BsfApp bapp = BsfFromBytes("BOSY&\0\0\0\xf4\xb8\xfa\xfa\xbf\xde\xb0\x01\xbe\x11\0\0\0\xcd\x80\xeb\xfe$!AHello, world!$!F\n\0");
    // BsfExec(&bapp);
}
