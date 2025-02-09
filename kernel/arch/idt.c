#include "idt.h"

IDTEntry IDTEntries[256];
IDTPointer IDTPtr;

void IDTDefaultHandler()
{
    vga = secondvgabuf;
    U8 c = TTYCurrentColor;
    TTYSetColor(Red, Black);
    TTYPrintPos("Unhandled interrupt", 80-20, 4);
    TTYCurrentColor = c;
}

void IDTSetEntry(U8 num, U32 base, U16 selector, U8 type_attr)
{
    IDTEntries[num].offset0 = base & 0xFFFF;
    IDTEntries[num].offset1 = (base >> 16) & 0xFFFF;
    IDTEntries[num].selector = selector;
    IDTEntries[num].zero = 0;
    IDTEntries[num].type_attr = type_attr;
}
void IDTInit() {
    IDTPtr.limit = sizeof(IDTEntry) * 256 - 1;
    IDTPtr.base = (U32)&IDTEntries;
    for (int i = 0; i < 256; i++) {
        IDTSetEntry(i, (U32)IDTDefaultHandler, 0x08, 0x8E);
    }
    asm volatile("lidt %0" : : "m"(IDTPtr));
}