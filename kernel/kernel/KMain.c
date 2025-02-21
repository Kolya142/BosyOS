#include <drivers/keyboard.h>
#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <user/syscall.h>
#include <misc/wordgen.h>
#include <arch/getreg.h>
#include <drivers/vga.h>
#include <drivers/pit.h>
#include <misc/string.h>
#include <misc/cp517.h>
#include <misc/time.h>
#include <arch/beep.h>
#include <libs/bosz.h>
#include <misc/meml.h>
#include <user/user.h>
#include <user/bsf.h>
#include <arch/cpu.h>
#include <arch/gdt.h>
#include <arch/sys.h>
#include <arch/pic.h>
#include <libs/tty.h>

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
            "snake\n"
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
                if (tail[i] == snake) {
                    for (U32 i = 0; i < 2000; ++i) {
                        vga[i] ^= 0xFF00;
                    }
                    TTYCursor = 0;
                    TTYUPrint("Game over!\n");
                    game = False;
                    break;
                }
            }
            TTYCursor = apple;
            TTYPrintC('$');
            TTYCursor = 0;
            TTYUPrintHex(score);
            MSleep(110);
        }
        KDogWatchPStart(0, "Main loop");
    }
    else if (!StrCmp(cmd, "music")) {
        KDogWatchPStart(0, "Music Player");

        U16 tones[] = { 3, 105, 91, 0, 91, 94, 0, 94, 90, 0, 90, 95, 0, 96, 90, 0, 90, 94, 0, 94, 90, 0, 90, 96, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 0, 0, 184, 187, 198, 0, 0, 186, 184, 186, 0, 0, 186, 184, 0, 0, 184, 187, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 0, 0, 184, 187, 198, 186, 184, 186, 186, 184, 184, 187, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 0, 0, 184, 187, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 0, 0, 184, 187, 0, 0, 198, 186, 184, 0, 0, 186, 186, 184, 184, 187, 198, 186, 184, 186, 186, 184, 184, 187, 198, 186, 184, 186, 0, 186, 0, 184, 184, 0, 187, 0, 198, 186, 0};
        TTYUPrint("$!0$*A<Song> $!F$*0");
        U8 tone = 0;

        U32 z = calculate_shash(PITTime);

    
        for (;;) {
            KDogWatchPTick(0);
            U16 t = tones[tone];

            TTYRawPrint("012345679ABCDEF"[(t >> 8) >> 8], Black, White);
            TTYRawPrint("012345679ABCDEF"[(t >> 8) & 15], Black, White);
            TTYRawPrint("012345679ABCDEF"[(t & 0xff) >> 8], Black, White);
            TTYRawPrint("012345679ABCDEF"[(t & 0xff) & 15], Black, White);
            TTYCursor -= 4;

            U32 c = TTYCursor;
            TTYCursor += 80 - TTYCursor % 80;
            WordGen();
            TTYCursor = c;
            
            if (t != 0) {
                Beep(t);
            }
    
            MSleep(60);
            tone++;
            vga[(z >> 16) & 0xfff] = z & 0xffff;
            z = calculate_shash((c >> 1) ^ PITTime);
            if (tone >= (sizeof(tones) / sizeof(U16))) {
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
