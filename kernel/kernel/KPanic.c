#include <kernel/KDogWatch.h>
#include <kernel/KPanic.h>
#include <misc/string.h>
#include <drivers/pit.h>
#include <arch/beep.h>
#include <libs/tty.h>
#include <libs/mem.h>
#include <arch/cpu.h>
#include <arch/sys.h>

U0 KPanic(const String msg, Bool reboot)
{
    for (U8 i = 0; i < 255; ++i) {
        KDogWatchPEnd(i);
    }
    U32 start = PITTime;
    stat U16 tones[] = {400, 300, 100, 400, 200, 400, 250};
    stat U8 tind = 0;
    U32 ecode = *(int*)msg;
    {
        U32 l = StrLen(msg) - StrLen(msg)%4;
        for (U32 i = 0; i < l; ++i) {
            ecode ^= *(int*)&msg[i];
        }
    }
    
    TTYUPrint("$!A$*CKernelPanic: ");

    TTYUPrint(msg);
    TTYUPrint("\n");

    TTYUPrint("$!0$*FSystem: BosyOS, Time at panic: ");
    TTYUPrint("\n");
    {
        U32i *t = (U32i*)&start;
        const String hex = "0123456789ABCDEF";
        TTYRawPrint(hex[t->u8[3] >> 4], Blue, White);
        TTYRawPrint(hex[t->u8[3] & 15], Blue, White);
        TTYRawPrint(hex[t->u8[2] >> 4], Blue, White);
        TTYRawPrint(hex[t->u8[2] & 15], Blue, White);
        TTYRawPrint(hex[t->u8[1] >> 4], Blue, White);
        TTYRawPrint(hex[t->u8[1] & 15], Blue, White);
        TTYRawPrint(hex[t->u8[0] >> 4], Blue, White);
        TTYRawPrint(hex[t->u8[0] & 15], Blue, White);
        TTYUPrint("\n");
    }
    {
        U32i *t = (U32i*)&ecode;
        const String hex = "0123456789ABCDEF";
        TTYRawPrint(hex[t->u8[3] >> 4], Blue, White);
        TTYRawPrint(hex[t->u8[3] & 15], Blue, White);
        TTYRawPrint(hex[t->u8[2] >> 4], Blue, White);
        TTYRawPrint(hex[t->u8[2] & 15], Blue, White);
        TTYRawPrint(hex[t->u8[1] >> 4], Blue, White);
        TTYRawPrint(hex[t->u8[1] & 15], Blue, White);
        TTYRawPrint(hex[t->u8[0] >> 4], Blue, White);
        TTYRawPrint(hex[t->u8[0] & 15], Blue, White);
        TTYUPrint("\n");
    }
    while (PITTime - start < 15000) {
        {
            U32 d = (15000 - PITTime - start);
            U32i *t = (U32i*)&d;
            const String hex = "0123456789ABCDEF";
            TTYRawPrint(hex[t->u8[3] >> 4], Blue, White);
            TTYRawPrint(hex[t->u8[3] & 15], Blue, White);
            TTYRawPrint(hex[t->u8[2] >> 4], Blue, White);
            TTYRawPrint(hex[t->u8[2] & 15], Blue, White);
            TTYRawPrint(hex[t->u8[1] >> 4], Blue, White);
            TTYRawPrint(hex[t->u8[1] & 15], Blue, White);
            TTYRawPrint(hex[t->u8[0] >> 4], Blue, White);
            TTYRawPrint(hex[t->u8[0] & 15], Blue, White);
            TTYUPrint("\n");
            TTYCursor -= 80;
        }
        Beep(tones[tind]);
        tind = (tind + 1) % (sizeof(tones) / sizeof(U16));
    }
    if (reboot) {
        PowerReboot();
    }
    CpuHalt();
}