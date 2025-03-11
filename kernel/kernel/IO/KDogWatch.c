#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <arch/x86/cpu/cpu.h>
#include <lib/IO/TTY.h>
#include <arch/x86/cpu/io.h>
#include <arch/x86/sys/idt.h>

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
    TTYUPrint("$!A[DogWatch]$!F:$!E");
    TTYUPrint(str);
    TTYUPrint("$!C at $!B");
    TTYUPrintHex(KDogWatchTicks);
    TTYUPrint("$!F\n");
    if (panic) {
        KPanic("KDogWatchLog panic", True);
    }
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