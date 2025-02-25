// Kernel
#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <kernel/KMem.h>

// Drivers
#include <drivers/keyboard.h>
#include <drivers/vga.h>
#include <drivers/pit.h>

// Miscellaneous
#include <misc/syscall.h>
#include <misc/wordgen.h>
#include <misc/vfiles.h>
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

// FileSystem
#include <fs/BOTFS.h>
#include <fs/vfs.h>
#include <fs/ufs.h>

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
    HeapInit();
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
    UFSInit();
    VFSInit();
    VFilesInit();
    DATInit();
    BOTFSInit();
    SleepM(2000);
    TTYClear();
    TTYCursor = 0;
    TTYUPrint(
        "\n"
        "$!EBosyOS control codes$!F: $*F$!0See *mezex.txt*$*0$!F\n");
    programtest();
    // KDogWatchPStart(0, "Main loop"); No Watching
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
        "$!5mus pong testufs testheap$!F\n"
        "$!6testbfs ls cat fwrite$!F\n"
        "$!7touch fclean$!F\n"
        "$!5Features$!F:\n"
        "All Commands: esc-exit\n"
        "$!Asound$!F - $!Balt$!F-play, $!Blshift$!F-save, $!Brshift$!F-load\n"
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
        PrintF("%w");
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
    PrintF("$!BPIT$!F:    %x\n", PITTime);
    PrintF("$!BYear$!F:   %d\n", SystemTime.year);
    PrintF("$!BMonth$!F:  %d\n", SystemTime.month);
    PrintF("$!BDay$!F:    %d\n", SystemTime.day);
    PrintF("$!BHour$!F:   %d\n", SystemTime.hour);
    PrintF("$!BMinute$!F: %d\n", SystemTime.minute);
    PrintF("$!BSecond$!F: %d\n", SystemTime.second);
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
    
    U8 *Record = MAlloc(555);
    MemSet(Record, 1, 555);
    U32 i = 0;
    Bool start = False;
    for (;;) {
        if (KBState.keys['\x1b']) {
            break;
        }
        else if (KBState.keys[ASCIIPAlt]) {
            for (U32 j = 0; j < 555 && Record[j] != 1; ++j) {
                if (Record[j])
                    BeepSPC(Record[j], 3);
                KDogWatchPTick(0);
                SleepM(5);
            }
        }
        else if (KBState.keys[ASCIIPLshift]) {
            BOTFSCreate("mysound", 555);
            BOTFSWrite("mysound", Record, 0, 555);
        }
        else if (KBState.keys[ASCIIPRshift]) {
            BOTFSRead("mysound", Record, 0, 555);
        }
        else if (!(KBState.SC & 0x80)) {
            if (KBState.Key > 0x80) {
                Record[i >> 2] = 0;
            }
            else {
                U8 note = KeyToNote[KBState.Key%sizeof(KeyToNote)];
                TTYUPrintHex(note);
                BeepSPC(note, 3);
                Record[i >> 2] = note;
                start = True;
            }
        }
        else if ((KBState.SC & 0x80)) {
            Record[i >> 2] = 0;
        }
        TTYUPrintHex(i >> 2);
        if (start) {
            i = (i+1) % (555 << 2);
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
U0 CPong() {
    KDogWatchPEnd(0);
    I32 p1 = 10;
    I32 p2 = 10;
    I32 ballx = RandomU() % 10 + 40;
    I32 bally = RandomU() % 10 + 10;
    I32 ballvx = (RandomU() & 1) ? 1 : -1;
    I32 ballvy = (RandomU() & 1) ? 1 : -1;
    Bool game = True;
    while (game) {
        for (U32 i = 0; i < 2000; ++i) {
            vga[i] = 0xF7B0;
        }
        
        for (U32 y = p1 - 3; y <= p1 + 3; ++y) {
            vga[y * 80 + 0] = 0x00DB;
        }
        for (U32 y = p2 - 3; y <= p2 + 3; ++y) {
            vga[y * 80 + 79] = 0x00DB;
        }
        vga[ballx + bally * 80] = 0x09DB;
        ballx += ballvx;
        bally += ballvy;

        if (bally == 24 || bally == 0) {
            ballvy = -ballvy;
            BeepSPC(45, 30);
        }
        if (ballx == 1 && bally >= p1 - 3 && bally <= p1 + 3) {
            ballvx = -ballvx;
            BeepSPC(45, 30);
        }
        if (ballx == 78 && bally >= p2 - 3 && bally <= p2 + 3) {
            ballvx = -ballvx;
            BeepSPC(45, 30);
        }
        if (KBState.keys['w']) {
            --p1;
        }
        if (KBState.keys['s']) {
            ++p1;
        }
        p2 += (bally - p2) / 5;
        if (ballx == 79 || ballx == 0) {
            ballx = 4;
            bally = 4;
            ballvx = 1;
            ballvy = 1;
            BeepSPC(25, 30);
        }
        if (KBState.keys['\x1b']) {
            game = False;
        }
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
    else if (!StrCmp(cmd, "ls")) {
        String ffiles[32] = {0};
        BOTFSList(ffiles, 32);
        for (U32 i = 0; i < 32; ++i) {
            if (!ffiles[i]) continue;
            BOTFSFileStat fstat = BOTFSStat(ffiles[i]);
            PrintF("%s(%C %p %X) ", ffiles[i], fstat.flags, fstat.start, fstat.size);
        }
    }
    else if (StrStartsWith(cmd, "fwrite")) {
        String file = &cmd[7];
        BOTFSFileStat fstat = BOTFSStat(file);
        if (fstat.exists) {
            U8 *buf = MAlloc(fstat.size);
            KBRead(buf, fstat.size);
            BOTFSWrite(file, buf, 0, fstat.size);
            PrintF(buf);
            MFree(buf);
        }
        else {
            PrintF("$!CError: file $!F\"%s\"$!C not found$!F", file);
        }
    }
    else if (StrStartsWith(cmd, "fclean")) {
        PrintF("Do you want to remove all files?\nIf yes rewrite this: \"$!7The quick brown fox jumps over a lazy dog$!F\"\n(Note: gray is 7)?");
        Char buf[48];
        KBRead(buf, 48);
        if (!StrCmp(buf, "$!7The quick brown fox jumps over a lazy dog$!F")) {
            BOTFSFormat();
            PrintF("$!ACleaned$!F");
        }
    }
    else if (StrStartsWith(cmd, "touch")) {
        String file = &cmd[6];
        BOTFSFileStat fstat = BOTFSStat(file);
        if (fstat.exists) {
            PrintF("$!CError: file $!F\"%s\"$!C exists$!F", file);
        }
        else {
            BOTFSCreate(file, 32);
        }
    }
    else if (StrStartsWith(cmd, "cat")) {
        String file = &cmd[4];
        BOTFSFileStat fstat = BOTFSStat(file);
        if (fstat.exists) {
            U8 *buf = MAlloc(fstat.size);
            BOTFSRead(file, buf, 0, fstat.size);
            PrintF(buf);
            MFree(buf);
        }
        else {
            PrintF("$!CError: file $!F\"%s\"$!C not found$!F", file);
        }
    }
    else if (!StrCmp(cmd, "words")) {
        CWords();
    }
    else if (!StrCmp(cmd, "triangle")) {
        CTriangle();
    }
    else if (!StrCmp(cmd, "testufs")) {
        U8 buf[1] = {0};
        UFSRead("dev", "random", buf, 1);
        PrintF("Test: %x\n", buf[0]);
        U8 ubuf[1] = {4};
        UFSRead("home", "test", ubuf, 4);
        PrintF("Test home: %s", ubuf);
    }
    else if (!StrCmp(cmd, "testbfs")) {
        BOTFSCreate("test", 10);
        BOTFSCreate("trash", 1);
        PrintF("File: %B\n", BOTFSFind("test") != 0xFFFFFFFF);
        BOTFSWrite("test", "sus\n", 0, 4);
        U8 buf[4] = {0};
        BOTFSRead("test", buf, 0, 4);
        String ffiles[5] = {0};
        BOTFSList(ffiles, 5);
        PrintF("existing files: ");
        for (U32 i = 0; i < 5; ++i) {
            if (!ffiles[i]) continue;
            PrintF("%s ", ffiles[i]);
        }
        PrintF("\nData: %s\n", buf);
        BOTFSDelete("trash");
        PrintF("Deleted trash: %B\n", BOTFSFind("trash") == 0xFFFFFFFF);    
    }
    else if (!StrCmp(cmd, "testheap")) {
        TTYUPrint("Test:");
        TTYUPrintHex((U32)MAlloc(3));
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
    else if (!StrCmp(cmd, "pong")) {
        CPong();
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
                    PrintF("$!CNo code found$!F.\n");
                    return;
                }
                DATRead(0x6266, ubuf, 0, 500);
                if (ubuf[0] == '\0') {
                    PrintF("$!CEmple code$!F.\n");
                    return;
                }
                PrintF("$!ACode$!F: %s\n\n", ubuf);
                BFInterpret(ubuf);
            }
            else {
                PrintF("$!DEnter bf code$!F:");
                Char ubuf[500];
                KBRead(ubuf, 500);
                DATAlloc(0x6266);
                DATWrite(0x6266, ubuf, 0, 500);
                BFInterpret(ubuf);
                PrintF("\n$!ACode saved...$!F");
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
    else if (!StrCmp(cmd, "$!Csnd$!F")) {
        U16 tones[] = { 61, 59, 57, 55, 55, 57, 59, 61, 63, 64, 64, 65, 66, 66, 65, 64, 63, 62, 64, 61, 61, 62, 63, 61, 59, 57, 56, 56, 57, 58, 59, 63, 61, 59, 56, 57, 59, 60, 62 };
        U16 durations[] = { 136, 136, 272, 272, 272, 272, 272, 136, 136, 136, 272, 136, 136, 272, 136, 136, 136, 136, 272, 136, 272, 272, 272, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136, 136 };
        
        for (U32 i = 0; i < sizeof(tones) / 2; ++i) {
            for (U8 v = 0; v < 2; v++) {
                BeepSPC(tones[i], durations[i] / 2);
                SleepM(10);
            }
            if (i % 4 == 0) BeepSPC(tones[i] - 12, durations[i] / 2);
            SleepM(1000/70);
            KDogWatchPTick(0);
            if (KBState.keys['\x1b']) {
                break;
            }
        }
    }
    else if (!StrCmp(cmd, "mus")) {
        U16 tones[] = { 79, 78, 76, 76, 78, 57, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 69, 62, 79, 59, 78, 62, 76, 66, 76, 62, 57, 78, 62, 66, 62, 74, 59, 62, 76, 66, 69, 62, 57, 62, 66, 62, 59, 62, 66, 69, 62, 76, 59, 64, 78, 67, 79, 64, 59, 64, 76, 67, 73, 64, 57, 74, 64, 67, 64, 76, 57, 64, 69, 67, 69, 64, 57, 78, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 62, 79, 59, 78, 62, 76, 66, 76, 62, 78, 57, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 69, 62, 79, 59, 78, 62, 76, 66, 76, 62, 57, 62, 78, 66, 74, 62, 59, 62, 76, 66, 69, 62, 57, 62, 66, 62, 59, 62, 66, 62, 76, 59, 64, 78, 67, 79, 64, 59, 64, 76, 67, 73, 64, 57, 64, 74, 67, 76, 64, 57, 69, 64, 74, 67, 76, 64, 77, 58, 76, 62, 74, 65, 72, 70, 69, 70, 72, 53, 60, 77, 65, 60, 76, 55, 74, 62, 74, 65, 72, 62, 74, 57, 72, 60, 72, 65, 60, 72, 53, 60, 69, 65, 70, 60, 72, 53, 60, 77, 65, 60, 79, 55, 77, 62, 76, 65, 74, 62, 74, 57, 76, 60, 77, 65, 60, 77, 57, 60, 79, 65, 81, 60, 82, 58, 82, 65, 81, 69, 65, 79, 58, 65, 77, 69, 79, 65, 81, 57, 81, 60, 79, 65, 60, 77, 57, 60, 74, 65, 72, 60, 74, 57, 77, 60, 77, 65, 76, 60, 57, 76, 60, 78, 64, 78, 66, 50, 57, 78, 74, 50, 57, 71, 47, 54, 74, 78, 74, 47, 54, 69 };
        U16 durations[] = { 1010, 249, 246, 246, 249, 0, 264, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 0, 249, 0, 249, 0, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 0, 249, 246, 0, 246, 0, 249, 264, 248, 246, 249, 249, 246, 246, 0, 249, 0, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 0, 249, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 0, 249, 0, 249, 0, 246, 0, 246, 0, 249, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 248, 246, 249, 249, 246, 246, 249, 0, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 248, 0, 246, 0, 249, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 0, 744, 246, 249, 0, 264, 248, 0, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 248, 0, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 0, 249, 249, 0, 246, 0, 246, 0, 249, 0, 512, 246, 0, 0, 249, 0, 0, 249, 246, 246, 0, 0, 249 };

        for (U32 i = 0; i < sizeof(tones) / 2; ++i) {
            BeepSPC(tones[i], durations[i]);
            SleepM(1000/120);
            KDogWatchPTick(0);
            if (KBState.keys['\x1b']) {
                break;
            }
        }
    }
    else {
        TTYUPrint("Unk ");
        TTYUPrint(cmd);
    }
}

U0 mainloop() {
    Char buffer[50] = {0};
    KDogWatchPEnd(0);
    for (;;) {
        TTYUPrint("$!A\\$ $!F");
        KBRead(buffer, 50);
        // KDogWatchPPlay(0);
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
