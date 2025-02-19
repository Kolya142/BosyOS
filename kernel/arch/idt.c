#include <arch/io.h>
#include <arch/cpu.h>

volatile U32 IDTNothingCounter = 0;

INT_DEF(IDTNothing) {
    INT_START;
    IDTNothingCounter++;
    INT_RETURN;
}

IDTR IDTRO;
IDTDescriptor IDTTable[128];

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
    IDTRO.lim  = (U16)(sizeof(IDTDescriptor) * 128 - 1);
    for (U8 i = 0; i < 128; i++) {
        IDTSet(i, IDTNothing, 0x08, 0x8E);
    }
    __asm__ volatile(
        "lidt %0\n"
        "sti" : : "m"(IDTRO)
    );
}