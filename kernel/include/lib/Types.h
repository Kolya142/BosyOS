#pragma once
#include <stdint.h>
typedef uint8_t  U8;
typedef int8_t   I8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef int16_t  I16;
typedef int32_t  I32;

typedef void     U0;

typedef float    F32;

typedef U8       Bool;
typedef U8       Byte;
typedef U8       Char;

typedef U0*      Ptr;
typedef Char*    String;

typedef union {
    U8 u8[2];
    I8 i8[2];
    U16 u16;
    I16 i16;
} U16i;
typedef union {
    U8 u8[4];
    I8 i8[4];
    U16 u16[2];
    I16 i16[2];
    U32 u32;
    I32 i32;
} U32i;

extern Bool Debugging;

#define True 1
#define False 0
#define Null 0
#define NULL Null

#define asmv asm volatile
#define asmV __asm__ volatile

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define swapx(a, b) {a = a ^ b; b = a ^ b; a = a ^ b;}

typedef struct FrameBuffer {
    U32 width;
    U32 height;
    U32 *buffer;
    U32 pitch;
    U8 bpp;
} FrameBuffer;

extern FrameBuffer fb0;

#define WIDTH fb0.width
#define HEIGHT fb0.height

#define I8_MIN (-0x80)
#define I8_MAX 0x7F
#define U8_MIN 0
#define U8_MAX 0xFF
#define I16_MIN (-0x8000)
#define I16_MAX 0x7FFF
#define U16_MIN 0
#define U16_MAX 0xFFFF
#define I32_MIN (-0x80000000)
#define I32_MAX 0x7FFFFFFF


typedef enum ASCIIP {
    ASCIIPCtrl = 0x80,
    ASCIIPLshift,
    ASCIIPRshift,
    ASCIIPPtrsc,
    ASCIIPAlt,
    ASCIIPCapsLock,
    ASCIIPF1,
    ASCIIPF2,
    ASCIIPF3,
    ASCIIPF4,
    ASCIIPF5,
    ASCIIPF6,
    ASCIIPF7,
    ASCIIPF8,
    ASCIIPF9,
    ASCIIPF10,
    ASCIIPNumLock,
    ASCIIPScroll,
    ASCIIPHome,
    ASCIIPUp,
    ASCIIPPgup,
    ASCIIPLeft,
    ASCIIPCenter,
    ASCIIPRight,
    ASCIIPEnd,
    ASCIIPDown,
    ASCIIPPgdown,
    ASCIIPInsert,
    ASCIIPCBlack,
    ASCIIPCDBlue,
    ASCIIPCDGreen,
    ASCIIPCDCyan,
    ASCIIPCDRed,
    ASCIIPCDPurple,
    ASCIIPCBrown,
    ASCIIPCGray,
    ASCIIPCDGray,
    ASCIIPCBlue,
    ASCIIPCGreen,
    ASCIIPCCyan,
    ASCIIPCRed,
    ASCIIPCPurple,
    ASCIIPCYellow,
    ASCIIPCWhite,
    ASCIIPNextRaw,
} ASCIIP;