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

#define stat static volatile
#define asmv asm volatile
#define asmV __asm__ volatile

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define swapx(a, b) {a = a ^ b; b = a ^ b; a = a ^ b;}

uint32_t drvcall(uint32_t d1, uint32_t d2, uint32_t id, uint32_t value);
uint32_t syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
extern uint32_t _syscall(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);

void exit(uint32_t code);

float sin(float x);
float cos(float x);
float sqrt(float x);
float atan2(float y, float x);
float fmodf(float a, float b);