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
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/Time.h>
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
            "help cls hlt\n"
            "time echo test\n"
            "reboot poweroff music\n"
            "snake text sound\n"
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
                    if (KBState.Key == 'w') {
                        TTYCursor -= 80;
                    }
                    else if (KBState.Key == 'a') {
                        TTYCursor--;
                    }
                    else if (KBState.Key == 's') {
                        TTYCursor += 80;
                    }
                    else if (KBState.Key == 'd') {
                        TTYCursor++;
                    }
                    else if (KBState.Key == 'd') {
                        TTYCursor++;
                    }
                    VgaCursorSet(TTYCursor, 0);
                }
                else if (KBState.Key < 0x80) {
                    TTYUPrintC(KBState.Key);
                }
                if (KBState.Key == ASCIIPCtrl) {
                    text ^= 1;
                }
                if (KBState.Key == '\x1b') {
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
            81, 83, 85, 87, 89, 91, 93, 95
        };
        U8 Record[3556];
        MemSet(Record, 0, 3556);
        U32 i = 0;

        for (;;) {
            if (KBState.Key == '\x1b') {
                break;
            }
            if (KBState.Key == ASCIIPAlt) {
                for (U32 j = 0; Record[j]; ++j) {
                    BeepSPC(Record[j], 1);
                }
            }
            if (KBState.SC & 0x80) {
                U8 note = KeyToNote[KBState.SC%sizeof(KeyToNote)];
                TTYUPrintHex(note);
                BeepSPC(note, 1);
                Record[i>>2] = note;
            }
            TTYUPrintHex(i>>2);
            TTYCursor -= TTYCursor % 80;
            i = (i+1) % (sizeof(Record) << 2);
            KDogWatchPTick(0);
            SleepM(10);
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
        U32 apple = calculate_shash(PITTime) % 1300 + 500;
        U32 score = 0;
        U8 dir = 4;
        for (U32 i = 0; i < 2000; ++i) {
            vga[i] = 0xF02E;
        }
        Bool game = True;
        while (game) {
            if (KBState.Key == 'w') dir = 1;
            else if (KBState.Key == 'a') dir = 2;
            else if (KBState.Key == 's') dir = 3;
            else if (KBState.Key == 'd') dir = 4;

            for (U8 i = 4; i >= 1; --i) {
                vga[tail[i]] = 0xF02E;
                tail[i] = tail[i-1];
            }
            tail[0] = snake;

            if (snake == apple) {
                score++;
                vga[apple] = 0xF02E;
                apple = calculate_shash(PITTime) % 1300 + 500;
            }

            if (dir == 1      && ((snake - 80) < 55555)) snake -= 80;
            else if (dir == 2 && (snake - 1) % 80 != 79) snake -= 1;
            else if (dir == 3 && ((snake + 80) < 2000))  snake += 80;
            else if (dir == 4 && ((snake +1) % 80 != 0)) snake += 1;

            for (U8 i = 0; i < 5; ++i) {
                TTYCursor = tail[i];
                TTYPrintC('@');
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
            if (KBState.Key == '\x1b') {
                game = False;
            }
            TTYCursor = apple;
            TTYPrintC('$');
            TTYCursor = 0;
            TTYUPrintHex(score);
            SleepM(110);
        }
        KDogWatchPStart(0, "Main loop");
    }
    else if (!StrCmp(cmd, "music")) {
        KDogWatchPStart(0, "Music Player");

        U16 tones[] = { 79, 78, 76, 76, 78, 57, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 69, 62, 79, 59, 78, 62, 76, 66, 76, 62, 57, 78, 62, 66, 62, 74, 59, 62, 76, 66, 69, 62, 57, 62, 66, 62, 59, 62, 66, 69, 62, 76, 59, 64, 78, 67, 79, 64, 59, 64, 76, 67, 73, 64, 57, 74, 64, 67, 64, 76, 57, 64, 69, 67, 69, 64, 57, 78, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 62, 79, 59, 78, 62, 76, 66, 76, 62, 78, 57, 62, 66, 62, 59, 62, 66, 62, 57, 62, 66, 69, 62, 79, 59, 78, 62, 76, 66, 76, 62, 57, 62, 78, 66, 74, 62, 59, 62, 76, 66, 69, 62, 57, 62, 66, 62, 59, 62, 66, 62, 76, 59, 64, 78, 67, 79, 64, 59, 64, 76, 67, 73, 64, 57, 64, 74, 67, 76, 64, 57, 69, 64, 74, 67, 76, 64, 77, 58, 76, 62, 74, 65, 72, 70, 69, 70, 72, 53, 60, 77, 65, 60, 76, 55, 74, 62, 74, 65, 72, 62, 74, 57, 72, 60, 72, 65, 60, 72, 53, 60, 69, 65, 70, 60, 72, 53, 60, 77, 65, 60, 79, 55, 77, 62, 76, 65, 74, 62, 74, 57, 76, 60, 77, 65, 60, 77, 57, 60, 79, 65, 81, 60, 82, 58, 82, 65, 81, 69, 65, 79, 58, 65, 77, 69, 79, 65, 81, 57, 81, 60, 79, 65, 60, 77, 57, 60, 74, 65, 72, 60, 74, 57, 77, 60, 77, 65, 76, 60, 57, 76, 60, 78, 64, 78, 66, 50, 57, 78, 74, 50, 57, 71, 47, 54, 74, 78, 74, 47, 54, 69 };

        U16 durations[] = { 1010, 249, 246, 246, 249, 0, 264, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 0, 249, 0, 249, 0, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 0, 249, 246, 0, 246, 0, 249, 264, 248, 246, 249, 249, 246, 246, 0, 249, 0, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 0, 249, 246, 0, 246, 0, 249, 264, 0, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 248, 246, 249, 249, 246, 246, 249, 264, 248, 246, 0, 249, 0, 249, 0, 246, 0, 246, 0, 249, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 248, 246, 249, 249, 246, 246, 249, 0, 264, 248, 0, 246, 0, 249, 249, 246, 0, 246, 0, 249, 264, 248, 0, 246, 0, 249, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 0, 744, 246, 249, 0, 264, 248, 0, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 248, 0, 246, 249, 0, 249, 0, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 249, 0, 249, 246, 0, 246, 0, 249, 0, 264, 0, 248, 0, 246, 0, 249, 249, 0, 246, 0, 246, 0, 249, 0, 512, 246, 0, 0, 249, 0, 0, 249, 246, 246, 0, 0, 249 };

        TTYUPrint("$!0$*A<Song> $!F$*0");
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
    
            BeepSPC(tones[i], durations[i]);
            SleepM(50);
            i++;
            vga[(z >> 16) & 0xfff] = z & 0xffff;
            z = calculate_shash((c >> 1) ^ tones[i]);
            if (i >= (sizeof(tones) / sizeof(U16))) {
                break;
            }
            if (KBState.Key == '\x1b') {
                break;
            }
        }
        KDogWatchPStart(0, "Main loop");
    }
    else if (!StrCmp(cmd, "test")) {
        KDogWatchLog("Test", False);
    }
    else {
        TTYUPrint("Unk ");
        TTYUPrint(cmd);
    }
}

U0 mainloop() {
    Bool lk = False;
    Char buffer[50] = {0};
    U32 bufferi = 0;
    TTYUPrint("$!A\\$ $!F");
    for (;;) {
        if (!(KBState.SC & 0x80) && !lk) {
            lk = True;
            if (!KBState.Key) continue;
            if (KBState.Key < 0x80) {
                if (KBState.Key == '\b') {
                    if (bufferi) {
                        buffer[bufferi--] = 0;
                    }
                    TTYUPrintC(KBState.Key);
                }
                else if (KBState.Key == '\r') {
                    TTYUPrintC('\n');
                    // TTYUPrint(buffer);
                    if (bufferi) {
                        termrun(buffer);
                    }

                    MemSet(buffer, 0, sizeof(buffer));
                    bufferi = 0;
                    TTYUPrint("\n$!A\\$ $!F");
                }
                else {
                    Char key = KBState.Key;
                    if (KBState.Shift) key = ToUpper(key);
                    if (bufferi < sizeof(buffer) - 1) {
                        buffer[bufferi++] = key;
                        TTYUPrintC(key);
                    }
                }
            }
        }
        else if ((KBState.SC & 0x80) && lk) {
            lk = False;
        }
        KDogWatchPTick(0);
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
}
