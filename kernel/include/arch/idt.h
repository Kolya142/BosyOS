#pragma once
#include <lib/Types.h>

typedef struct IDTDescriptor {
    U16 low;
    U16 select;
    U8  zero;
    U8  attr;
    U16 high;
} __attribute__((packed)) IDTDescriptor;
typedef struct INTRegs3 {
    U32 edi, esi, ebp, useresp, ebx, edx, ecx, eax;
    U32 eflagsp;
    U32 ds, es, gs, fs;
    U32 eip, cs, eflags;
    U32 esp, ss;
} __attribute__((packed)) INTRegs3;
typedef struct INTRegs {
    U32 edi, esi, ebp, useresp, ebx, edx, ecx, eax;
    U32 eflagsp;
    U32 ds, es, gs, fs;
    U32 eip, cs, eflags;
} __attribute__((packed)) INTRegs;
typedef struct IDTR {
    U16 lim;
    U32 base;
} __attribute__((packed)) IDTR;
extern volatile U32 IDTNothingCounter;
U0 IDTSet(U8 i, Ptr p, U8 select, U8 f);

U0 IDTInit();
// remember: all this code is inserted before EVERY interrupt handler
#define INT_DEF0(fn) \
    __attribute__((naked)) U0 fn() { \
        asmv("cli\n" \
            "push %fs\n" \
            "push %gs\n" \
            "push %es\n" \
            "push %ds\n" \
            "pushf\n" \
            "pusha\n" \
                       \
            "mov $0x10, %ax\n" \
            "mov %ax, %ds\n" \
            "mov %ax, %es\n" \
            "mov %ax, %fs\n" \
            "mov %ax, %ss\n" \
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
            "pop %ds\n" \
            "pop %es\n" \
            "pop %gs\n" \
            "pop %fs\n" \
            "iret\n"); \
    } \

#define INT_DEF(fn) \
    INT_DEF0(fn) \
    U0 fn##_impl(INTRegs *regs)

// remember: all this code is inserted before EVERY interrupt handler
#define INT_DEF3(fn) \
    INT_DEF0(fn) \
    U0 fn##_impl(INTRegs3 *regs)

#define INT_START  asm("")
#define INT_RETURN asm("")