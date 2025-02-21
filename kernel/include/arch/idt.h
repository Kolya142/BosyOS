#pragma once
#include <lib/Types.h>

typedef struct IDTDescriptor {
    U16 low;
    U16 select;
    U8  zero;
    U8  attr;
    U16 high;
} __attribute__((packed)) IDTDescriptor;
typedef struct IDTR {
    U16 lim;
    U32 base;
} __attribute__((packed)) IDTR;
extern volatile U32 IDTNothingCounter;
U0 IDTSet(U8 i, Ptr p, U8 select, U8 f);

U0 IDTInit();

#define INT_DEF(fn) __attribute__((naked)) U0 fn()
#define INT_START  asmv("pusha\npushf")
#define INT_RETURN POut(0x20, 0x20);asmv("popf\npopa\niret")