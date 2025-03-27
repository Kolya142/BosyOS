#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <arch/x86/cpu/cpu.h>
#include <lib/IO/TTY.h>
#include <arch/x86/cpu/io.h>
#include <fs/ramfs.h>
#include <drivers/time/pit.h>
#include <arch/x86/sys/idt.h>
#include <kernel/KTasks.h>
#include <stdarg.h>

typedef struct {
    Char name[32];
    Bool active;
    U32  update;
} KDogWatchProfile;

volatile U32 KDogWatchTicks = 0;
volatile KDogWatchProfile Profiles[255] = {0};

#define ERR(name, text) INT_DEF(name) { \
    KDogWatchLog(text, False); \
    KPanic("x86 exception", False); \
}
ERR(DWErr00, "#DIVIDE BY 0");
ERR(DWErr0D, "#GP");
ERR(DWErr05, "#BOUND");
ERR(DWErr08, "#DOUBLE FAULT");
ERR(DWErr0C, "#STACK SEGMENT FAULT");
INT_DEF(DWErr0E) {
    U32 cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));
    SerialPrintF("#Page fault code: %p\n", cr2);
    KDogWatchLog("#Page fault", False);
    if (TaskTail) TaskClose();
    else KPanic("x86 exception", False);
}
U0 KDogWatchInit() {
    // IDTSet(0x00, DWErr00, 0x08, 0x8E);
    // IDTSet(0x0D, DWErr0D, 0x08, 0x8E);
    // IDTSet(0x0E, DWErr0E, 0x08, 0x8E);
    // IDTSet(0x05, DWErr05, 0x08, 0x8E);
    // IDTSet(0x08, DWErr08, 0x08, 0x8E);
    // IDTSet(0x0C, DWErr0C, 0x08, 0x8E);
    KDogWatchLog("Init", False);
}

U0 KDogWatchLog(const String str, Bool panic) {
    U32 t = TTYCurrent;
    TTYCurrent = 4;
    PrintF("$!A[DogWatch]$!F:$!E%s$!C at $!B%X$!F\n", str, PITTime);
    TTYCurrent = t;
    SerialPrintF("[DogWatch]:%s at %X", str, PITTime);
    if (panic) {
        KPanic("KDogWatchLog panic", True);
    }
}
U0 KDogWatchLogF(const String format, ...) {
    KDogWatchLog(format, False);
    SerialPrintF("[DogWatch]:");

    va_list args;
    va_start(args, format);
    VPrintF(0, format, args);
    TTYUPrintC(0, '\n');
    va_end(args);

    SerialPrintF(" at %X", PITTime);
}

U0 KDogWatchTick() {
    KDogWatchTicks++;
    for (U8 i = 0; i < 255; ++i) {
        if (!Profiles[i].active)
            continue;
        if (KDogWatchTicks >= Profiles[i].update + 8000) {
            // TTYUPrint("$!A[DogWatch]$!F:$!E");
            // TTYUPrint("Slowly ");
            // TTYUPrint(Profiles[i].name);
            // TTYUPrint("$!C at $!B");
            // TTYUPrintHex(KDogWatchTicks);
            // TTYUPrint("$!F\n");
            Profiles[i].update = KDogWatchTicks;
        }
    }
    if (TaskTail && TaskHead && TaskTail->esp - TaskTail->regs.useresp > 8192) {
        // KDogWatchLogF("Task %d used too many stack memory", TaskTail->id);
        // KDogWatchStackTrace();
        // TaskKill(TaskTail->id);
        // TaskNext();
    }
}
U0 KDogWatchStackTrace() {
    struct stackframe {
        struct stackframe* ebp;
        U32 eip;
    };
    PrintF("Stack trace\n");
    struct stackframe *stk = (struct stackframe*)__builtin_frame_address(0);
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
    // TTYUPrint("$!A[DogWatch]$!F:$!E");
    // TTYUPrint("Init ");
    // TTYUPrint(Profiles[id].name);
    // TTYUPrint("$!C at $!B");
    // TTYUPrintHex(KDogWatchTicks);
    // TTYUPrint("$!F\n");
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