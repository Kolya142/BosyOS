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

struct stackframe {
    struct stackframe* ebp;
    U32 eip;
};

U0 KPanic(const String msg, Bool reboot)
{ // FIXME
    // for (U8 i = 0; i < 255; ++i) {
    //     KDogWatchPEnd(i);
    // }
    // U32 start = PITTime;
    // static U16 tones[] = {400, 300, 100, 400, 200, 400, 250};
    // static U8 tind = 0;
    // U32 ecode = *(U32*)msg;
    // {
    //     for (U32 i = 0; i < 64000; ++i) {
    //         VVRM[i] ^= 15;
    //     }
    // }
    // {
    //     U32 l = StrLen(msg) - StrLen(msg)%4;
    //     for (U32 i = 0; i < l; ++i) {
    //         ecode ^= *(U32*)&msg[i];
    //     }
    // }
    // VRM = VVRM;
    
    // PrintF("$!A$*CKernelPanic: %s\n$!0$*F\nTime at panic: %d\nMessage Hash: %x\n", msg, start, ecode);
    // PrintF("Stack trace:\n");
    // struct stackframe *stk;
    // asmv("movl %%ebp, %0" : "=r"(stk));
    // for (U32 frame = 0; stk && stk->eip && frame < 10; ++frame) {
    //     PrintF("<- %p\n", stk->eip);
    //     stk = stk->ebp;
    // }
    // while (PITTime - start < 15000) {
    //     {
    //         U32 d = (15000 - PITTime - start);
    //         ((TTY*)TTYs.arr)[TTYCurrent].pty->cursor -= ((TTY*)TTYs.arr)[TTYCurrent].pty->width;
    //     }
    //     Beep(tones[tind]);
    //     tind = (tind + 1) % (sizeof(tones) / sizeof(U16));
    // }
    // if (reboot) {
    //     PowerReboot();
    // }
    // CpuHalt();
}