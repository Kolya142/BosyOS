#include <lib/BosZ.h>
#include <lib/MemLib.h>

Data DeCompress(const Byte *input, U32 len) {
    U32 i = 0;

    #define getc input[i++]

    if (getc != 0x8D) {
        return (Data) {.data = Null, .len = 0};
    }

    Byte *output = MAlloc(64);
    U32 oi = 0;
    U32 oc = 64;

    #define putc(v) {if (oi > oc) {output = MReAlloc(output, oc, oc <<= 1);} output[oi++] = v;}

    while (1) {
        U8 count = getc;
        if (count == 0) break;

        if (count == 0xFF) {
            U8 raw_size = getc;
            for (U8 j = 0; j < raw_size; j++) {
                putc(getc);
            }
        } else {
            U8 value = getc;
            for (U8 j = 0; j < count; j++) {
                putc(value);
            }
        }
    }
    return (Data) {
        .data = MReAlloc(output, oc, oi),
        .len = oi
    };
}