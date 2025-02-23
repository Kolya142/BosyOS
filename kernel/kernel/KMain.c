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
    else if (!StrCmp(cmd, "apple")) {
        TTYClear();
        TTYCursor = 0;
        KDogWatchPStart(0, "Bad Apple Player");

        U16 tones[] = { 63, 51, 63, 51, 65, 58, 65, 58, 51, 66, 51, 66, 68, 58, 58, 68, 51, 70, 51, 58, 58, 70, 75, 51, 51, 75, 58, 73, 58, 73, 70, 51, 51, 58, 70, 58, 51, 63, 51, 58, 63, 58, 51, 70, 70, 51, 58, 68, 68, 58, 66, 51, 66, 51, 65, 58, 58, 65, 63, 47, 63, 47, 54, 65, 54, 65, 66, 47, 66, 47, 68, 54, 68, 54, 70, 47, 47, 54, 70, 54, 47, 68, 68, 47, 54, 66, 54, 66, 49, 65, 65, 49, 63, 56, 63, 56, 49, 65, 65, 49, 66, 56, 66, 56, 65, 50, 50, 65, 63, 58, 63, 58, 62, 50, 62, 50, 65, 58, 65, 58, 63, 51, 51, 63, 65, 58, 65, 58, 66, 51, 51, 66, 58, 68, 58, 68, 70, 51, 51, 58, 58, 70, 51, 75, 51, 75, 73, 58, 58, 73, 51, 70, 51, 58, 58, 70, 63, 51, 51, 58, 63, 58, 70, 51, 70, 51, 58, 68, 68, 58, 51, 66, 66, 51, 65, 58, 58, 65, 63, 47, 63, 47, 54, 65, 54, 65, 47, 66, 66, 47, 54, 68, 68, 54, 47, 70, 47, 54, 54, 70, 68, 47, 47, 68, 54, 66, 54, 66, 49, 65, 49, 56, 56, 65, 49, 66, 49, 56, 56, 66, 50, 68, 50, 58, 58, 68, 50, 70, 50, 58, 58, 70, 63, 51, 63, 51, 58, 65, 65, 58, 66, 51, 51, 66, 68, 58, 68, 58, 51, 70, 51, 58, 58, 70, 51, 75, 75, 51, 58, 73, 58, 73, 51, 70, 51, 58, 70, 58, 63, 51, 51, 58, 63, 58, 51, 70, 70, 51, 58, 68, 58, 68, 51, 66, 51, 66, 65, 58, 58, 65, 63, 47, 47, 63, 65, 54, 65, 54, 47, 66, 66, 47, 54, 68, 68, 54, 47, 70, 47, 54, 54, 70, 47, 68, 47, 68, 66, 54, 54, 66, 49, 65, 49, 65, 63, 56, 56, 63, 65, 49, 49, 65, 66, 56, 56, 66, 65, 50, 65, 50, 63, 58, 63, 58, 62, 50, 62, 50 };

        U16 durations[] = { 0, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 102, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0, 102, 0 };

        String images = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x80\x0f\x80\x0f\x80\x0f\x80\x0f\x88\x0f\x80\x0f\x80\x07\0\x03\0\x01\0\x01\0\x01\0\x01\0\0\0\0\xf8\x0f\xf8\x1f\xf8\x1f\xf8?\xf8?\xfc?\xf8?\xf8\x1f\xf0\x0f\xf0\x0f\xf0\x0f\xe0\x0f\xe0\x0f\xc0\x0f\xfc\x1f\xfc\x1f\xfc?\xfe?\xfe?\xfe?\xfc?\xfc\x1f\xfc\x1f\xf8\x1f\xf8\x0f\xf8\x0f\xfc\x0f\xfc\x0f\xfd\xff\xf8?\xfc\x7f\xfc\x7f\xfc\x7f\xfc\x7f\xfc\x7f\xf8\x7f\xf8?\xf8?\xf0?\xf0?\xf0?\xf0?\xff\xdf\xfe\x1f\xfe\x1f\xfe?\xfe?\xfe?\xfe\x1f\xfe\x1f\xfc\x1f\xfc\x1f\xf8\x0f\xfc\x0f\xfc\x0f\xfc\x0f\xfcG\xfc\x07\xfe\x07\xfe\x0f\xfe\x0f\xfe\x1f\xfe\x1f\xfe\x1f\xfe\x0f\xfe\x07\xfe\x07\xfe\x13\xfe\x13\xfc\x11\xff\xff\xff\x9f\xff\x8f\xff\x07\xff\x07\xff\x07\xff\x87\xff\x87\xff\x87\xf3\x8f\xf1\x07\xfc\x07\xf8\x07\xf8\x07\xff\x9f\xff\x87\xff\x03\xff\x03\xff\x03\xff\x03\xff\x83\xff\x83\xf8\x83\xf8\xc5\xfc\x03\xfc\x07\xfc\x07\xfc\x03\xff\xcf\xff\x83\xff\x03\xff\x03\xff\x01\xff\x01\xff\x01\xfc\x81\xfc\x81\xfc\x01\xfe\x03\xfe\x03\xfc\x03\xfc\x81\xfc\x0f\xfe\x0f\xfe\x07\xfc\x07\xfc\x03\xfc\x03\xff\x03\xff\x03\xff\x03\xff\x87\xff\x87\xe3\x87\xe3\x83\xf0\x03\xff\xff\xff\xff\xff\xff\xff\xff\xff\xf7\xffw\xffg\xff'\xff\x0f\xfe\x0f\xfe\x06\xfe\x06\xfe\x02\xfe\0\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xfe\x1f\xfe\x1f\xfe\x1f\xfe?\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x1f\xfe\x1f\xfc\x1f\xfe\x1f\xfe?\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01\xff\x03\xff\x01\xff\0\x8f\0\x87\x01\xc2\x01\xe2\x03\xe0\x03\xe0\x01\xe0\0\xc0\0\0\0\0\0\0\x03\xfe\x03\xff\x01\xfe\x01\xfe9\xf89\xf8\x1d\xf5\x07\xff\x03\xff\x03\x7f\x02\x7f\x06\xff\r\xff\xff\xff\x0e>\x1e\x7f\x1f\xfd\x07\xfc\x03\xff\x01\xff\0\xff\0\xff\x01\xff\x01\xdf\0\xff\0?\0?\0?\0\0\0\0\0\0\x01\xc0\0\xc0\x01\xe0\0\xe0\0p\x1f\xfe\0\xff\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x01\x80\0\xc0\0\xc0\x07\xf0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x80\0\x80\0\x80\x07\xf0\0\x10\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x01\x80\0\x80\0@\x01\xf0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0 \00\0 \0\x14\0\x7e\0\x06\0\0 \0\xe0\0\xe0\0\xe0\0\0\0\0\0\0\0\0 \0 \0 \08\0\x18\0\0@\0b\0\xfe\0\xfe\0\xfe\0\0\0\0\0\0\0\0\0\0`\0 \0 \0p@\x10`\0p\0\xfe\0\xfe\0\xfe\0\0\0\0\0\0 \0p\00\0p\0<\0\x7c\08\0\0@\0\xc0\0\xc1\0\xfd\0\0\0\0\0\0\0\0\0\0\x01\x01\xe3\x03\xff\x02?\x01\x18\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x06\0\x02\0\x02\0\x02\0\0\0\0\0\0\0\0\0\0\0\0\0\xff\xcf\xff\xcf\xff\xcf\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xe7\xff\xe7\xff\xe7\xff\xe7\xff\xc7\xff\xe7\xff\xe7\xff\xe7\xff\xd7\xff\xff\xff\xff\xff\xfb\xff\xf3\xff\xf3\xff\xf1\xff\xf1\xff\xf1\xff\xf1\xff\xe1\xff\xf1\xff\xf1\xff\xf1\xff\xf1\xff\xe1\xff\xff\xff\xff\xff\xff?\xff?\xff\x1f\xff\x0f\xff\x1f\xff?\xff?\xff?\xff?\xff\x1f\xff\x1f\xfe\x1f\xff\xbf\xff\x1f\xfe\x1f\xfe\x1f\xff\x1f\xff\x1f\xf0\x0f\xf2\x0f\xff\x0f\xff\x0f\xff\x0f\xff\x0f\xff\x1f\xfe\x1f\xff\xff\xff\xff\xff\x1f\xff\x0f\xff\x0f\xff\x8f\xff\x8f\xff\x0f\xfe\x0f\xfc\x07\xf9\x07\xfd\x07\xff\x07\xff\x07\xff\xf7\xff\xc3\xff\xc3\xff\x01\xff\x81\xff\xc1\xfeA\xfe\xc1\xfe\0\xfe\0\xfe\0\xff\0\xff\0\xff\0\xff\xe0\xff\xc0\xff\xc0\xff\0\xff\0\xff\xc0\xff\xc0\xf9\xc0\xf1\xc0\xf9\xc0\xf9\xc0\xfc\0\xfe\0\xfe\0\xff\xff\xff\xff\xff\xff\xf7\xff\xf3\xff\xf9\xff\xfd\xff\xf8\x7f\xf0\x7f\xf0\x7f\xf0\x7f\xf8?\xfc\x1f\xfe\x0f\xff\xff\xfe?\xfe\x0f\xfe\x1f\xfc\x1f\xfe?\xfe\x7f\xcc\x1e\xc0\x18\xb0\0\xfc7\xfe?\xfc?\xfc?\xff\xff\xff\xff\xfc\x1f\xfc\x1f\xfc?\xfc?\xfe\x7f\xdc;\x0c\x11`\0\xfc?\xfc?\xfc?\xfc?\xff\xff\xfe/\xfe\x07\xfc\x0f\xfc\x0f\xfe\x1f\xfe?\xe4\x7f\xc4\x1e\xf0\x0c\xf0 \xf84\xfc\x7f\xfc\x7f\xff\xf8\xff\xe0\xff\xe0\xff\x80\xff\xc0\xff\xe0\xff\xe0\xe1\xe8\xf0\xf1\xfc\xe1\xfe\x01\xfe\0\xff\0\xff\xc3\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xf0\x7f\xf8\x7f\xfc\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xfd\xff\xf8\x7f\xfc\x0f\xfc/\xfc\x1f\xfe\x7f\xff\xff\xff\xff\0\0\0\0`\0\0\0\0\0\0\0\0\0\0\0\0  \0\x1cD\x0e\xe8\x06l\x03\xa4\0\0\0\0\0\0\0\0\0\0\x03\0\x03\x80\x01\x80\0@\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x04\0\x01\x80\x01\xc0\x01\xc0\0\x80\0@\0@\0\0\0\0\0\0\0\0\0\0\0\0\x01\0\x03\x80\x03\x80\x03\x80\x01\x80\x01\x80\x01\xc0\x01\xa0\x03\x80\x03\x80\x03\xc0\0\0\0\0\0\x80\x01\xc0\x01\xc0\x1d\xc0\x07\x80\x03\x80\x03\x80\x01\x80\x03\xc0\x03\x80\x03\x80\x03\xc0\0\0\0\0\x03\x80\x03\xc0\x07\xc0\x03\xc0\x01\x80\x03\xe0\x03\xe0\x03\xe0\x03\xe0\x03\xe0\x01\xf0\x03\xe0\0\0\0\0\0\xe0\0\xf0\x01\xf0\0\xf8\0\xf0\x03\xf0\x07\xf8\x07x\0\x7c\0\xfe\0\xfc\0\xf8\0\0\0\0\0\0\0\0\0\0\0\0\0?\0\x1f\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0p\x01\xf8\x01\xf8\x01\xf8\x01p1p\x89xt\xf8\x15\xf8\x07\xff\x04\xf8\0\xf8\0\xf8\0\0\0\xf0\0\xf0\0\xf0\x01\xf8\x10ph0\pZ\xfeC\xfa\x03\xf2\x01\xf2\0\xf8\0\xf0\0\0\x01\x80\x03\xe0\x03\xe0\x03\xe0\x01\xa0a\xa0\x9f\xee\x03\xd9\x03\xc0\x03\xc0\x03\xc0\x03\xc0\x03\xc0\0\0\x01\x80\x03\xe0\x03\xe0\x03\xe0\x01\xa0a\xa4\xef\xf6\xa7\xed\x03\xc8\x05\xc0\t\x80\x03\xe0\x03\xe0\0\0\x03\xc0\x03\xe0\x03\xe0\x03\xe0\x01\xa0A\xa2\xb7\xedw\xeeG\xe4\x05\xa0\t\x983\xcc\x03\xc0\0\0\x03\xc0\x03\xe0\x03\xe0\x03\xe0\x01\xa0A\xa2\xb3\xed\xbb\xdc\x13\xc8\x17\xc0\x1f\xf83\xcc\x03\xc0\xf8\x07\xf8\x07\xfc\x07\x7c\x06\xfe!\x90\t\x90\t0\x0f\xf0\x0f\xf8\x1f\xf0\x0f\xf4\x0fD&\x0c0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xff\xff\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x06\0\x07\0\x0f\0\0\0  \xf0\x800\00\x800\04\0p\x01\xf8\0\xfa\0\xc0\0`\xc0@\xe1\x80\x01\0!\0!\0#\0#\0\x03\x80\x03\xc0\x03\x80\x02\x80\x02\0\00\x01\b\0\0\x03\0\x03\0#\x80!@'\xc0/\0#\x80\x03\x80\x03\0\x02\x80\0\x80\0\0\0\xc0\x01\xc0\x01\xc0\0\x80\x01\xe2C\x9c\x03\x90G\xc0C\x80C\x80\x0b\xc0c\xd0\x07\xc0\0\0\x03\x80\x07\x80\x0f\x90\x1f\xc0\x1fC\x1fL?\xec\x7f\xc4?\xcc?\xe4\0F\0D\0D\0\08\0>\0\x7e\0\x7e\0>\0\x7e\0\x7e\0>\0\x7c\0\x7e\0\x7e\0\xfe\0\xff\0\x04\0\x1f\x01\x1f\0\x1f\0?\0\x1f\0\x1f\0\x1f\0\x0f\x80\x1f\0\x1f\x80\x1f\x80\x1f\x80?\x80\0\00\0x\0\xfc\0\xfc\0\xfc\0\xfc\0\xfc\0\xfc\0\xfd\xe0\xff\xf0\xff\xf8\xff\xfc\xff\xf0\x02\0\x0e\0\x1f\0\x1f\0\x1f\0?\xa0\x1f\0\x1f\x80\x1f\0\x1f\0\x1f\xf0\x1f\xf8?\xfc?\xfc\0\0\x1c\0\x7c\0\x7e\0\x7e\0>\0\x7c\0\x7c\0\x7c\0>\0?\xc0\x7f\xf0\x7f\xf8\x7f\xf8\0\0\b\0<\0\x7e\0\x7e\0>\0>\0<\08\0\x7e\0\x7e\0\x7f\xe0\x7f\xf8\x7f\xfc\0\0\0\0\x80\0\xc0\0\xc0\0\xf0\0\xf0\0\xe0\0\xe0\0\xe0\0\xe0\0\xe0\0\xc0\0\xf0\0\0\0\0\0\0\0\0\0\0\x80\x01\x80\x01\x80\x01\x80\x03\x80\x03\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x02`\x0b\xe0\x07\xc0\x0f\x80\x06\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xc0\0\xc0\0@\0`\x1cX\x1f\xf8\x07\xf0\x02\0\0\0\0\0\0\0\x11\xc0\x17\xc0\x17\xc0\x0f\xc0\x01\xc7\0\xff\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xf8\xff\xe0\xff\x800\0A\x80a\xc0E\x80A\x80\xc1\xb0\xc1\xdb\xc0\xe0\xc1\x80A\xc0A\xc0C\xc0G\xe0O\xe0\x1f\xe7\0\0\0\0\x01\x80A\xc0S\x80\xc3\x80\xc1\xe0\xc1\xcf\xc1\xf8\xc1\xc0@\xc0A\xe0a\xe0c\xe0\0\0\0\x10\0x\x18x9\xf00<0=0<0?0>0\x1c\x10\x1c\x18>\b\x7e\x03\xfe\x03\xfe\x03\xfe\x03\xfe\x03\xfe\x03\xfc\x03\xfc\x03\xfc\x03\xfc\x03\xfc\x03\xfc\x03\xfc\x03\xfc\x01\xfe\0\xff\0\xff\0\xff\0\xff\0\xff\0\xff\0\xff\0\xff\x03?\x03?\x03?\x03?\x03?\x01?\0\xff\0\xff\0\xff\0\xff\x01?\x03?\x01\x7f\x01\xff\x03?\x03?\x03?\x03?\x03?\x03?\0\xff\0\xff\0\xff\0\xff\0\x7f\x01?\x01\x1f\x01?\x01\x0f\x01\x1f\x017\x01?\x01?\x03?\x01\xff\x01\xff\x01\xff\x01\xff\x01\xff\x01\xff\x01\xff\x03\xff\x03\xff\x02\x7f\x06\x7f\x02\x7f\x06\xff\x02\x7f\x07\xff\x07\xff\x07\xff\x01\xff\t\xff\x11\xff\xf8\xff\b\xff\n\xff\t\x7f\t\xff\b\xff\t\xff\r\xff\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x07\0\x01\0\0\0\0\0\0\0\0\0\0\0\0\x01\0\x03\x80\x03\x80\x01\x80\x01\xc0\x01\xc0\x01\xc0\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\0\0\x01\0\x03\x80\x03\x80\x01\x80\x01\x80\x01\xc0\x01\xc0\x03\xc0\x0f\xc0\x19\xc0\x01\xc0\x01\x80\x01\x80\0\0\x01\x80\x03\x80\x03\x80\x01\x80\x01\xc0\x01\xc0#\xc0\x0f\xc0\x01\xc0\x01\xc0\x01\xc0\x01\x80\x01\x80\0\0\x03@\x01\xc0\x01\xc0\x01\xc0\x13\xc0\x13\xc0\x13\xc0\x0f\xc0\x01\xc0\x01\xa0\x01\xc0\x03\xc0\x03\xc0\0\0\x01@\x01\xc0\x01\xc0\x01\x80\x13\x80\x1b\xc0\x13\xc0\x0f\xc0\x03\xb8\x03\x80\x03\xc0\x03\xc0\x03\xc0\0\0\x02@\x03\xc0\x05\xb0\x05\xa0\x07\xa0\x07\xf0\x07\xe0\x07\xe0\x03\xc0\x01\x80\x03\xc0\x03\xc0\x03\xc0\xdc\xfc\xde\xfc\x7e\xfc\x7f\xfe\xfe\x7f\xff\x7e\xff\xbe\x7f\xfe\x7f\xfc\x7f\xfc?\xfcs\xdc;\xdc\x1b\xf8\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01\xffA\xffa\xffa\xffs\xffs\xff\xd3\xff\xff\xff\x7f\xff;\xff\x7b\xffy\xff9\xff\x10\xff\x18\xff\x18\xffy\xffs\xff\x7b\xff\x7b\xffs\xff\xfb\xff\xbb\xffy\xff9\xffx\xff\x7e\x7f\x16?\0\0\x10\0\x18\b\x18\x0c\x18\x1c<\x1e\x7c\x1e\\x1e\x1c\x1c\x1c\x1c\x1c><><<\b\b\0\0\0\x80\x02@\x0ep\x0ep\x0ep\x0c0\x1c8\x1d\xb8\x1f\xf8\x1f\xf8\x1c8\x1c<\x1c>\x0c\0\x0c\0\x0c\0\x06p\x0ep\x0ep\x0ep\x0f\xf0\x07\xe0\x0f\xf0\x0ep\x0ep\x0c<\x1e<\x1f\xf8\x7f\xfc\x7f\xfc\x7f\xfc\x7f\xfc?\xfc>8<x<\xf8=\xf0>\xe0?\0?\xf8\x1f\xf9\xfc\0\xfc\0\xfc\0\xfc\x03\xfc\x03\xf8\0\xf0\x01\0\x01\0\x01\0\x01\0\x01\0\x01\0\x01\0\x03\xf8\0\xf8\0\xf8\0\xf80\xf08\xe0\x18\0\x18\0\x18\0\x18\0\x18\0\x18\0\x1c\0<\0<";
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
            BeepSPC(tones[i], dur);
            
            SleepM(50);
            i++;
            vga[(z >> 16) & 1999] = z & 0xffff;
            z = calculate_shash((c >> 1) ^ tones[i]);
            c = TTYCursor;
            TTYUPrintC('\n');
            TTYUPrintC('\n');
            TTYUPrintDec(((i>>1) % 88));
            TTYUPrint(" \n");
            const static U32 width = 16;
            const static U32 height = 14;
            for (U32 j = 0; j < height; ++j) {
                for (U32 I = 0; I < width/8; ++I) {
                    U32 frame = ((i>>1) % 88) * (width * height) / 8;
                    Byte byte = images[frame + (j * width / 8 + I)];
                    TTYUPrintC(byte & 0b00000001 ? '@' : '.');
                    TTYUPrintC(byte & 0b00000010 ? '@' : '.');
                    TTYUPrintC(byte & 0b00000100 ? '@' : '.');
                    TTYUPrintC(byte & 0b00001000 ? '@' : '.');
                    TTYUPrintC(byte & 0b00010000 ? '@' : '.');
                    TTYUPrintC(byte & 0b00100000 ? '@' : '.');
                    TTYUPrintC(byte & 0b01000000 ? '@' : '.');
                    TTYUPrintC(byte & 0b10000000 ? '@' : '.');
                }
                TTYUPrintC('\n');
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
