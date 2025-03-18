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
#if defined(__linux__)
#define __BUILD_OS__ "Linux"
#elif defined(__APPLE__)
#define __BUILD_OS__ "MacOS"
#elif defined(_WIN32)
#define __BUILD_OS__ "Windows"
#elif defined(__unix__)
#define __BUILD_OS__ "Unix-like"
#else
#define __BUILD_OS__ "Unknown OS"
#endif