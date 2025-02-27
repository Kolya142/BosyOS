#pragma once
#include <lib/Types.h>

typedef struct IDTDescriptor {
    U16 low;
    U16 select;
    U8  zero;
    U8  attr;
    U16 high;
} __attribute__((packed)) IDTDescriptor;
typedef struct INTRegs {
    U32 edi, esi, ebp, useresp, ebx, edx, ecx, eax;
    U32 eflagsp;
    U32 eip, cs, eflags;
} __attribute__((packed)) INTRegs;
typedef struct IDTR {
    U16 lim;
    U32 base;
} __attribute__((packed)) IDTR;
extern volatile U32 IDTNothingCounter;
U0 IDTSet(U8 i, Ptr p, U8 select, U8 f);

U0 IDTInit();

#define INT_DEF(fn) \
    __attribute__((naked)) U0 fn() { \
        asmv("cli\n" \
            "pushf\n" \
            "pusha\n" \
                       \
            "movl %esp, %eax\n" \
            "pushl %eax\n" \
            "call "#fn"_impl\n" \
            "addl $4, %esp\n" \
                            \
            "movb $0x20, %al;\n" \
            "outb %al, $0x20\n" \
            \
            "popa\n" \
            "popf\n" \
            "sti\n" \
            "iret\n"); \
    } \
    U0 fn##_impl(INTRegs *regs)

#define INT_START  /*asmv("pusha\npushf\n")*/asm("")
#define INT_RETURN /*POut(0x20, 0x20);asmv("popf\npopa\niret")*/asm("")