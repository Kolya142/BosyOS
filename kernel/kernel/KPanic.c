#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <lib/graphics/Graphics.h>
#include <drivers/time/pit.h>
#include <lib/memory/MemLib.h>
#include <lib/strings/String.h>
#include <drivers/sys/beep.h>
#include <arch/x86/cpu/cpu.h>
#include <arch/x86/sys/sys.h>
#include <lib/IO/TTY.h>
#include <drivers/sys/pci.h>

struct stackframe {
    struct stackframe* ebp;
    U32 eip;
};

U0 KPanic(const String msg, Bool reboot) {
    for (U8 i = 0; i < 255; ++i) {
        KDogWatchPEnd(i);
    }
    U32 start = PITTime;
    static U16 tones[] = {400, 300, 100, 400, 200, 400, 250};
    static U8 tind = 0;
    U32 ecode = *(U32*)msg;
    {
        for (U32 i = 0; i < 640*480; ++i) {
            VRM[i] ^= 15;
        }
    }
    {
        U32 l = StrLen(msg) - StrLen(msg)%4;
        for (U32 i = 0; i < l; ++i) {
            ecode ^= *(U32*)&msg[i];
        }
    }
    
    PrintF("$!A$*CKernelPanic: %s\n$!0$*F\nTime at panic: %d\nMessage Hash: %x\n", msg, start, ecode);
    PrintF("Stack trace:\n");
    struct stackframe *stk;
    asmv("movl %%ebp, %0" : "=r"(stk));
    for (U32 frame = 0; stk && stk->eip && frame < 10; ++frame) {
        PrintF("<- %p\n", stk->eip);
        stk = stk->ebp;
    }
    PrintF("Recovery shell\n");
    Char buf[512];
    MemSet(buf, 0, 512);
    PrintF(">>>");
    U32 i = 0;
    Bool r = 0;
    for (;;) {
        if (TTYRead(TTYCurrent, 0, buf, 512)) {
            PrintF("\"%s\"\n", buf);
            if (!StrCmp(buf, "reboot")) {
                PowerReboot();
            }
            else if (!StrCmp(buf, "lspci")) {
                PCIDevicesCheck();
            }
            else if (!StrCmp(buf, "off")) {
                PowerOff();
            }
            else if (!StrCmp(buf, "help")) {
                PrintF(
                    "reboot off\n"
                    "lspci help\n"
                );
            }
            PrintF(">>>");
            MemSet(buf, 0, 512);
            r = True;
        }
	    VRMFlush();
        ++i;
        if (i >= 1000000 && !r) {
            PowerReboot();
        }
    }
    if (reboot) {
        PowerReboot();
    }
    CpuHalt();
}
