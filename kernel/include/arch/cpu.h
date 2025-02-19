#pragma once
#include <libs/types.h>
void CpuHalt();
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