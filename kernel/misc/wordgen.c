#include <misc/wordgen.h>
#include <drivers/video/vga.h>
#include <drivers/misc/random.h>
#include <lib/time/Time.h>

U0 WordGenS(String str, U32 size)
{
    RTCUpdate();
    U32 v = RandomU();
    const String vowels = "AEIOUY";
    const String consonts = "BCDFGHJKLMNPQRSTVWXYZ";
    U8 args = v;
    v = RandomU();
    // args HASKELL 4;
    args >>= 4;
    args ^= v;
    Bool VowelStart = args & 1;
    Bool VowelEnd   = (args >> 1) & 1;
    U8 state        = VowelStart;
    for (int i = 0; i < size-VowelEnd-1; i++) {
        switch (state) {
            case 0: {
                str[i] = vowels[v%6];
                state = 1;
            } break;
            case 1: {
                str[i] = consonts[v%22];
                state = (PITTime^v>>2)&1 ? 2 : 0;
            } break;
            case 2: {
                str[i] = consonts[v%22];
                state = 0;
            } break;
        }
        v = RandomU();
    }
    if (VowelEnd) {
        str[size-2] = vowels[v%6];
    }
    str[size-1] = 0;
}

U0 WordGen()
{
    RTCUpdate();
    U32 v = RandomU();
    const String vowels = "AEIOUY";
    const String consonts = "BCDFGHJKLMNPQRSTVWXYZ";
    U8 args = v;
    v = RandomU();
    // args HASKELL 4;
    args >>= 4;
    args ^= v;
    Bool VowelStart = args & 1;
    Bool VowelEnd   = (args >> 1) & 1;
    U8 state        = VowelStart;
    for (int i = 0; i < 6+(v&6); i++) {
        switch (state) {
            case 0: {
                TTYUPrintC(TTYCurrent, vowels[v%6]);
                state = 1;
            } break;
            case 1: {
                TTYUPrintC(TTYCurrent, consonts[v%22]);
                state = (PITTime^v>>2)&1 ? 2 : 0;
            } break;
            case 2: {
                TTYUPrintC(TTYCurrent, consonts[v%22]);
                state = 0;
            } break;
        }
        v = RandomU();
    }
    if (VowelEnd) {
        TTYUPrintC(TTYCurrent, vowels[v%6]);
    }
}