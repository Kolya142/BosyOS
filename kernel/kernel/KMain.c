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

U0 termrun(const String cmd) {
    if (!StrCmp(cmd, "help")) {
        TTYUPrint(
            "Commands:\n"
            "help cls hlt gen\n"
            "time echo wsave rand\n"
            "reboot poweroff apple\n"
            "snake text sound wget\n"
        );
    }
    else if (!StrCmp(cmd, "cls")) {
        TTYClear();
        TTYCursor = 0;
    }
    else if (!StrCmp(cmd, "hlt")) {
        asmv("cli");
        CpuHalt();
    }
    else if (!StrCmp(cmd, "time")) {
        TTYUPrint("$!BPIT$!F:    "); TTYUPrintHex(PITTime); TTYUPrintC('\n');
        RTCUpdate();
        TTYUPrint("$!BYear$!F:   "); TTYUPrintDec(SystemTime.year); TTYUPrintC('\n');
        TTYUPrint("$!BMonth$!F:  "); TTYUPrintDec(SystemTime.month); TTYUPrintC('\n');
        TTYUPrint("$!BDay$!F:    "); TTYUPrintDec(SystemTime.day); TTYUPrintC('\n');
        TTYUPrint("$!BHour$!F:   "); TTYUPrintDec(SystemTime.hour); TTYUPrintC('\n');
        TTYUPrint("$!BMinute$!F: "); TTYUPrintDec(SystemTime.minute); TTYUPrintC('\n');
        TTYUPrint("$!BSecond$!F: "); TTYUPrintDec(SystemTime.second); TTYUPrintC('\n');
    }
    else if (StrStartsWith(cmd, "echo ")) {
        TTYUPrint(cmd+4);
    }
    else if (!StrCmp(cmd, "echo")) {
        TTYUPrint(".");
    }
    else if (!StrCmp(cmd, "rand")) {
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
            vga[(v >> 16) & 1999] = v & 0xffff;
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
    else if (!StrCmp(cmd, "gen")) {
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
    else if (!StrCmp(cmd, "reboot")) {
        PowerReboot();
    }
    else if (!StrCmp(cmd, "poweroff")) {
        KDogWatchPEnd(0);
        PowerOff();
    }
    else if (!StrCmp(cmd, "text")) {
        Bool lk = False;
        TTYCursor = 0;
        TTYUPrint("\npress ctrl to switch mode\npress esc to exit\n");
        Bool text = False;
        for (;;) {
            if (!(KBState.SC & 0x80) && !lk) {
                if (!text) {
                    if (KBState.keys['w']) {
                        TTYCursor -= 80;
                    }
                    else if (KBState.keys['a']) {
                        TTYCursor--;
                    }
                    else if (KBState.keys['s']) {
                        TTYCursor += 80;
                    }
                    else if (KBState.keys['d']) {
                        TTYCursor++;
                    }
                    VgaCursorSet(TTYCursor, 0);
                }
                else if (KBState.Key < 0x80) {
                    TTYUPrintC(KBState.Key);
                }
                if (KBState.keys[ASCIIPCtrl]) {
                    text ^= 1;
                }
                if (KBState.keys['\x1b']) {
                    break;
                }
                lk = True;
            }
            else if ((KBState.SC & 0x80) && lk) {
                lk = False;
            }
            KDogWatchPTick(0);
        }
    }
    else if (!StrCmp(cmd, "sound")) {
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
        
        U8 Record[3556];
        MemSet(Record, 0, 3556);
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
    else if (!StrCmp(cmd, "snake")) {
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
            vga[i] = 0xF03A;
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
                vga[tail[i]] = 0xF03A;
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
    else if (!StrCmp(cmd, "apple")) {
        TTYClear();
        TTYCursor = 0;
        KDogWatchPStart(0, "Bad Apple Player");

        U16 tones[] = { 63, 51, 63, 51, 65, 58, 65, 58, 51, 66, 51, 66, 68, 58, 58, 68, 51, 70, 51, 58, 58, 70, 75, 51, 51, 75, 58, 73, 58, 73, 70, 51, 51, 58, 70, 58, 51, 63, 51, 58, 63, 58, 51, 70, 70, 51, 58, 68, 68, 58, 66, 51, 66, 51, 65, 58, 58, 65, 63, 47, 63, 47, 54, 65, 54, 65, 66, 47, 66, 47, 68, 54, 68, 54, 70, 47, 47, 54, 70, 54, 47, 68, 68, 47, 54, 66, 54, 66, 49, 65, 65, 49, 63, 56, 63, 56, 49, 65, 65, 49, 66, 56, 66, 56, 65, 50, 50, 65, 63, 58, 63, 58, 62, 50, 62, 50, 65, 58, 65, 58, 63, 51, 51, 63, 65, 58, 65, 58, 66, 51, 51, 66, 58, 68, 58, 68, 70, 51, 51, 58, 58, 70, 51, 75, 51, 75, 73, 58, 58, 73, 51, 70, 51, 58, 58, 70, 63, 51, 51, 58, 63, 58, 70, 51, 70, 51, 58, 68, 68, 58, 51, 66, 66, 51, 65, 58, 58, 65, 63, 47, 63, 47, 54, 65, 54, 65, 47, 66, 66, 47, 54, 68, 68, 54, 47, 70, 47, 54, 54, 70, 68, 47, 47, 68, 54, 66, 54, 66, 49, 65, 49, 56, 56, 65, 49, 66, 49, 56, 56, 66, 50, 68, 50, 58, 58, 68, 50, 70, 50, 58, 58, 70, 63, 51, 63, 51, 58, 65, 65, 58, 66, 51, 51, 66, 68, 58, 68, 58, 51, 70, 51, 58, 58, 70, 51, 75, 75, 51, 58, 73, 58, 73, 51, 70, 51, 58, 70, 58, 63, 51, 51, 58, 63, 58, 51, 70, 70, 51, 58, 68, 58, 68, 51, 66, 51, 66, 65, 58, 58, 65, 63, 47, 47, 63, 65, 54, 65, 54, 47, 66, 66, 47, 54, 68, 68, 54, 47, 70, 47, 54, 54, 70, 47, 68, 47, 68, 66, 54, 54, 66, 49, 65, 49, 65, 63, 56, 56, 63, 65, 49, 49, 65, 66, 56, 56, 66, 65, 50, 65, 50, 63, 58, 63, 58, 62, 50, 62, 50 };

        U16 durations[] = { 0, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0 };

        String images = "          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n          \n       @@@\n       @@@\n       @@@\n       @@@\n       @@@\n       @@@\n       @@@\n       @@@\n        @@\n        @@\n@       @@\n@      @@@\n@      @@@\n@       @@\n@       @@\n         @\n         @\n         @\n         @\n          \n@@@     @@\n@@@     @@\n@@@     @@\n@@@     @@\n@@@      @\n@@@      @\n@@@      @\n@@       @\n@@        \n@@        \n@@@@    @@\n@@@@    @@\n@@@@    @@\n@@@@@   @@\n@@@@    @@\n@@@@    @@\n@@@     @@\n@@@      @\n@@@      @\n@@@      @\n@@@@   @@@\n@@@    @@@\n@@@   @@@@\n@@@@  @@@@\n@@@   @@@@\n@@@    @@@\n@@     @@@\n@@     @@@\n@@      @@\n@@      @@\n@@@   @@@@\n@@@   @@@@\n@@@  @@@@@\n@@@   @@@@\n@@@   @@@@\n@@@   @@@@\n@@    @@@@\n@@    @@@@\n@@     @@@\n@      @@@\n@@@   @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@   @@@@\n@@@   @@@@\n@@@    @@@\n@@@    @@@\n@@@    @@@\n@@@    @@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@@ @@@@\n@@@@  @@@@\n@@@@   @@@\n@@@@   @@@\n@@@@   @@@\n@@@@    @@\n@@@@    @@\n@@@@@@ @@@\n@@@@   @@@\n@@@@@ @@@@\n@@@@@ @@@@\n@@@@   @@@\n@@@@   @@@\n@@@@   @@@\n@@@@   @@@\n@@@@   @@@\n@@@@   @@@\n@@@@ @@@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@  @ @@@\n@@@  @ @@@\n@@@    @@@\n@@@    @@@\n@@@@@@@@@@\n@@@@  @@@@\n@@@@ @@@@@\n@@@@ @@@@@\n@@@@  @@@@\n@@@   @@@@\n@@@   @@@@\n@@@   @@@@\n@@@   @@@@\n@@@   @@@@\n@@@@@@@@@@\n@@@@  @@@@\n@@@@ @@@@@\n@@@@ @@@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@   @@@@\n@@@   @@@@\n@@@   @@@@\n@@@@  @@@@\n@@@@@@@@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@@  @@@\n@@@ @  @@@\n@@@@   @@@\n@@@@   @@@\n@@@@@@@@@@\n@@@@   @@@\n@@@@@ @@@@\n@@@@@ @@@@\n@@@@@  @@@\n@@@@   @@@\n@@@@   @@@\n@@@@   @@@\n@@@@   @@@\n@@@@   @@@\n@@@@ @@@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@   @@@@\n@@@    @@@\n@@@    @@@\n@@@   @@@@\n@@@@ @@@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@@  @@@@\n@@@    @@@\n@@@    @@@\n@@@    @@@\n@@@    @@@\n@@@@ @@ @@\n@@@@    @@\n@@@@   @@@\n@@@@@  @@@\n@@@@@  @@@\n@@@@@  @@@\n@@@@   @@@\n@@@@    @@\n@@@@@    @\n@@@@     @\n";
        TTYUPrint("$!0$*A<Bad Apple> $!F$*0");
        U8 i = 0;

        U32 z = calculate_shash(35535);
    
        for (;;) {
            KDogWatchPTick(0);

            TTYRawPrint("012345679ABCDEF"[(tones[i] >> 8) >> 8], Black, White);
            TTYRawPrint("012345679ABCDEF"[(tones[i] >> 8) & 15], Black, White);
            TTYRawPrint("012345679ABCDEF"[(tones[i] & 0xff) >> 8], Black, White);
            TTYRawPrint("012345679ABCDEF"[(tones[i] & 0xff) & 15], Black, White);
            TTYCursor -= 4;

            U32 c = TTYCursor;
            TTYCursor += 80 - TTYCursor % 80;
            WordGen();
            TTYCursor = c;
            U16 dur = durations[i];
            if (i > 0) {
                dur = (durations[i] + durations[i - 1]) / 2;
            }
            BeepSPC(tones[i], dur);
            
            SleepM(50);
            i++;
            vga[(z >> 16) & 1999] = z & 0xffff;
            z = calculate_shash((c >> 1) ^ tones[i]);
            c = TTYCursor;
            TTYUPrintC('\n');
            TTYUPrintC('\n');
            for (U32 j = 0; j < 10; j += 1) {
                for (U32 I = 0; I < 11; ++I) {
                    TTYUPrintC(images[I + j * 11 + (i % (210 / 10)) * (11 * 10)]);
                }
            }
            TTYCursor = c;
            if (i >= (sizeof(tones) / sizeof(U16))) {
                break;
            }
            if (KBState.keys['\x1b']) {
                break;
            }
        }
        KDogWatchPStart(0, "Main loop");
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
