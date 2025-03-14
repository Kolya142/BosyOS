#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <arch/x86/cpu/cpu.h>
#include <lib/IO/TTY.h>
#include <arch/x86/cpu/io.h>
#include <arch/x86/sys/idt.h>
#include <stdarg.h>

typedef struct {
    Char name[32];
    Bool active;
    U32  update;
} KDogWatchProfile;

volatile U32 KDogWatchTicks = 0;
volatile KDogWatchProfile Profiles[255] = {0};

#define ERR(name, text) INT_DEF(name) { \
    KDogWatchLog(text, True); \
}
ERR(DWErr0D, "#GP");
ERR(DWErr05, "#BOUND");
ERR(DWErr08, "#Double fault");
INT_DEF(DWErr0E) {
    U32 cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));
    PrintF("#Page fault code: %p\n", cr2);
    KDogWatchLog("#Page fault", True);
}
U0 KDogWatchInit() {
    IDTSet(0x0D, DWErr0D, 0x08, 0x8E);
    IDTSet(0x0E, DWErr0E, 0x08, 0x8E);
    IDTSet(0x05, DWErr05, 0x08, 0x8E);
    IDTSet(0x08, DWErr08, 0x08, 0x8E);
    KDogWatchLog("Init", False);
}

U0 KDogWatchLog(const String str, Bool panic) {
    U8 t = TTermID;
    TTYSwitch(3);
    PrintF("$!A[DogWatch]$!F:$!E%s$!C at $!B%X$!F\n", str, PITTime);
    TTYSwitch(t);
    SerialPrintF("[DogWatch]:%s at %X", str, PITTime);
    if (panic) {
        KPanic("KDogWatchLog panic", True);
    }
}
U0 KDogWatchLogF(const String format, ...) {
    U8 t = TTermID;
    TTYSwitch(3);

    PrintF("$!A[DogWatch]$!F:$!E");
    
    va_list args;
    va_list args2;
    
    va_start(args, format);
    va_copy(args2, args);
    
    VPrintF(format, args);

    va_end(args);
    
    PrintF("$!C at $!B%X$!F\n", PITTime);

    TTYSwitch(t);

    TTerm.render();
    Ptr rend = TTerm.render;
    TTerm.render = TTYRenderS;

    PrintF("$!A[DogWatch]$!F:$!E");

    VPrintF(format, args2);
    va_end(args2);

    PrintF("$!C at $!B%X$!F\n", PITTime);

    TTerm.render();
    TTerm.render = rend;
}

U0 KDogWatchTick() {
    KDogWatchTicks++;
    for (U8 i = 0; i < 255; ++i) {
        if (!Profiles[i].active)
            continue;
        if (KDogWatchTicks >= Profiles[i].update + 8000) {
            TTYUPrint("$!A[DogWatch]$!F:$!E");
            TTYUPrint("Slowly ");
            TTYUPrint(Profiles[i].name);
            TTYUPrint("$!C at $!B");
            TTYUPrintHex(KDogWatchTicks);
            TTYUPrint("$!F\n");
            Profiles[i].update = KDogWatchTicks;
        }
    }
}
U0 KDogWatchStackTrace() {
    struct stackframe {
        struct stackframe* ebp;
        U32 eip;
    };
    PrintF("Stack trace\n");
    struct stackframe *stk;
    asmv("movl %%ebp, %0" : "=r"(stk));
    for (U32 frame = 0; stk && stk->eip && frame < 10; ++frame) {
        PrintF("<- %p\n", stk->eip);
        stk = stk->ebp;
    }
}
U0 KDogWatchPStart(U8 id, const String name) {
    Profiles[id] = (KDogWatchProfile) {
        .active = True,
        .update = KDogWatchTicks,
    };
    U32 l = StrLen(name);
    MemCpy(Profiles[id].name, name, l > 32 ? 32 : l);
    TTYUPrint("$!A[DogWatch]$!F:$!E");
    TTYUPrint("Init ");
    TTYUPrint(Profiles[id].name);
    TTYUPrint("$!C at $!B");
    TTYUPrintHex(KDogWatchTicks);
    TTYUPrint("$!F\n");
}
U0 KDogWatchPTick(U8 id) {
    Profiles[id].update = KDogWatchTicks;
}
U0 KDogWatchPEnd(U8 id) {
    Profiles[id].active = False;
}
U0 KDogWatchPPlay(U8 id) {
    Profiles[id].active = True;
}