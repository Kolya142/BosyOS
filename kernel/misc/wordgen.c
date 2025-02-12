#include <misc/wordgen.h>

U0 WordGen()
{
    U32 v = PITTime;
    for (U32 i = 0; i < 6; i++) {
        v ^= PITTime;
        v <<= 2;
        v >>= 4;
        v ^= PITTime;
        MSleep(1);
    }
    v = calculate_shash(v);
    const char *vowels = "AEIOUY";
    const char *consonts = "QWRTPSDFGHJKLZXCVBNM";
    U8 args = v;
    v = calculate_shash(((v << 2) >> 3) ^ 0xB46D84E8);
    //   >>= - haskell icon
    args >>= 4;
    args ^= v;
    Bool VowelStart = args & 1;
    Bool VowelEnd   = (args >> 1) & 1;
    U8 state        = VowelStart;
    for (int i = 0; i < 6+(v&6); i++) {
        switch (state) {
            case 0: {
                TTYRawPrint(vowels[v%7], Green, Black);
                state = 1;
            } break;
            case 1: {
                TTYRawPrint(consonts[v%21], Green, Black);
                state = (PITTime^v>>2)&1 ? 2 : 0;
            } break;
            case 2: {
                TTYRawPrint(consonts[v%21], Green, Black);
                state = 0;
            } break;
        }
        v = calculate_shash(v ^ PITTime);
    }
    if (VowelEnd) {
        TTYRawPrint(vowels[v&7], Green, Black);
    }
    TTYRawPrint(' ', Black, Black);
}