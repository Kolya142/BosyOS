#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <misc/string.h>
#include <misc/meml.h>
#include <arch/cpu.h>
#include <libs/tty.h>
#include <arch/io.h>

typedef struct {
    Char name[32];
    Bool active;
    U32  update;
} KDogWatchProfile;

volatile U32 KDogWatchTicks = 0;
volatile KDogWatchProfile Profiles[255] = {0};

#define ERR(name, text) INT_DEF(name) { \
    INT_START; \
    asm volatile ( \
        "movw $0x10, %%ax \n" \
        "movw %%ax, %%ds  \n" \
        "movw %%ax, %%es  \n" \
        "movw %%ax, %%fs  \n" \
        "movw %%ax, %%gs  \n" \
        :::"ax" \
    ); \
    KDogWatchLog(text, True); \
    }
ERR(DWErr0D, "Cpu Error: 0x0D");
ERR(DWErr0E, "Cpu Error: 0x0E");
ERR(DWErr05, "Cpu Error: 0x05");

U0 KDogWatchInit() {
    IDTSet(0x0D, DWErr0D, 0x08, 0x8E);
    IDTSet(0x0E, DWErr0E, 0x08, 0x8E);
    IDTSet(0x05, DWErr05, 0x08, 0x8E);
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
        if (KDogWatchTicks > Profiles[i].update + 500) {
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