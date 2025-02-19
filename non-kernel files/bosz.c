#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
typedef uint8_t  U8;
typedef int8_t   I8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int16_t  I16;
typedef int32_t  I32;
typedef int64_t  I64;
typedef void     U0;

typedef U8       Bool;
typedef U8       Ukus;
typedef U8       Char;

typedef U0*      Ptr;
typedef Char*    String;
#define True 1
#define False 0
#define Null 0

U0 Compress(Ukus *input, Ukus *output) {
    FILE *in = fopen(input, "rb");
    FILE *out = fopen(output, "wb");

    if (!in || !out) {
        printf("Error: file error");
        exit(1);
    }

    putc(0x8D, out);

    Ukus buffer[253];

    for (;;) {
        U64 bytes_read = fread(buffer, 1, 253, in);
        if (bytes_read == 0) break;

        Bool raw = False;

        U8 prev = buffer[0];
        U8 count = 1;

        Ukus rawbuf[253];
        U8 rawind = 0;

        for (U64 i = 1; i < bytes_read; i++) {
            if (buffer[i] == prev && count < 0xFD) {
                if (raw) {
                    putc(0xFF, out);
                    putc(rawind, out);
                    fwrite(rawbuf, 1, rawind, out);
                    raw = False;
                    rawind = 0;
                }
                count++;
            } else {
                if (count > 1) {
                    putc(count, out);
                    putc(prev, out);
                } else {
                    raw = True;
                    rawbuf[rawind] = prev;
                    rawind++;
                    if (rawind >= 253) {
                        putc(0xFF, out);
                        putc(rawind, out);
                        fwrite(rawbuf, 1, rawind, out);
                        rawind = 0;
                    }
                }
                prev = buffer[i];
                count = 1;
            }
        }
        if (count > 1) {
            putc(count, out);
            putc(prev, out);
        }
        else {
            rawbuf[rawind] = prev;
            rawind++;
        }
        if (raw && rawind) {
            putc(0xFF, out);
            putc(rawind, out);
            fwrite(rawbuf, 1, rawind, out);
        }
    }
    putc(0, out);
    putc(0, out);
    fclose(in);
    fclose(out);
}

U0 DeCompress(Ukus *input, Ukus *output) {
    FILE *in = fopen(input, "rb");
    FILE *out = fopen(output, "wb");

    if (!in || !out) {
        printf("Error: file error\n");
        exit(1);
    }

    if (getc(in) != 0x8D) {
        printf("Error: invalid .BZ format\n");
        fclose(in);
        fclose(out);
        exit(1);
    }

    while (1) {
        U8 count = getc(in);
        if (count == 0) break;

        if (count == 0xFF) {
            U8 raw_size = getc(in);
            U8 raw_data[253];
            fread(raw_data, 1, raw_size, in);
            fwrite(raw_data, 1, raw_size, out);
        } else {
            U8 value = getc(in);
            for (U8 i = 0; i < count; i++) {
                putc(value, out);
            }
        }
    }

    fclose(in);
    fclose(out);
}

U64 main(U64 Argc, String *Argv) {
    if (Argc != 2) {
        printf("Usage: %s <input file>\n", Argv[0]);
        return 1;
    }
    U64 l = strlen(Argv[1]);
    if (Argv[1][l-1] == 'Z' && Argv[1][l-2] == 'B' && Argv[1][l-3] == '.') {
        String out = malloc(l - 2);
        snprintf(out, l - 2, "%s", Argv[1]);
        out[l - 3] = '\0';
        DeCompress(Argv[1], out);
        printf("%s -> %s\n", Argv[1], out);
        free(out);
        return 0;
    }
    String out = malloc(l+3);
    sprintf(out, "%s.BZ", Argv[1]);
    Compress(Argv[1], out);
    printf("%s -> %s\n", Argv[1], out);
    free(out);
}