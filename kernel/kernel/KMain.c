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
            "time echo\n"
            "reboot poweroff\n"
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
        TTYUPrint("$!BYear$!F:   "); TTYUPrintHex(SystemTime.year); TTYUPrintC('\n');
        TTYUPrint("$!BMonth$!F:  "); TTYUPrintHex(SystemTime.month); TTYUPrintC('\n');
        TTYUPrint("$!BDay$!F:    "); TTYUPrintHex(SystemTime.day); TTYUPrintC('\n');
        TTYUPrint("$!BHour$!F:   "); TTYUPrintHex(SystemTime.hour); TTYUPrintC('\n');
        TTYUPrint("$!BMinute$!F: "); TTYUPrintHex(SystemTime.minute); TTYUPrintC('\n');
        TTYUPrint("$!BSecond$!F: "); TTYUPrintHex(SystemTime.second); TTYUPrintC('\n');
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
        return;
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
