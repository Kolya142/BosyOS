#include <arch/x86/cpu/io.h>
#include <arch/x86/cpu/cpu.h>
#include <arch/x86/sys/idt.h>

volatile U32 IDTNothingCounter = 0;

__attribute__((naked)) U0 IDTNothing() {
    asmv("pusha\ncli");
    ++IDTNothingCounter;
    POut(0xA0, 0x20);
    POut(0x20, 0x20);
    asmv("sti\npopa\niret");
}

#define IDT_SIZE 255

IDTR IDTRO;
IDTDescriptor IDTTable[IDT_SIZE];

U0 IDTSet(U8 i, Ptr p, U8 select, U8 f) {
    IDTDescriptor *d = &IDTTable[i];
    d->low    = (U32)p & 0xFFFF;
    d->high   = (U32)p >> 16;

    d->select = select;
    d->zero   = 0;

    d->attr = f;
}

U0 IDTInit() {
    IDTRO.base = (U32)IDTTable;
    IDTRO.lim  = (U16)(sizeof(IDTDescriptor) * IDT_SIZE - 1);
    for (U8 i = 0; i < IDT_SIZE; ++i) {
        IDTSet(i, IDTNothing, 0x08, 0x8E);
    }
    __asm__ volatile(
        "lidt %0\n"
        "sti" : : "m"(IDTRO)
    );
}