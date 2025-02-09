#include <lib/types.h>
#include <kernel/mem.h>
#include <lib/screenlib.h>
#pragma once

typedef struct IDTEntry {
    U16 offset0;
    U16 selector;
    U8 zero;
    U8 type_attr;
    U16 offset1;
} __attribute__((packed)) IDTEntry;

typedef struct IDTPointer {
    U16 limit;
    U32 base;
} __attribute__((packed)) IDTPointer;


extern IDTEntry IDTEntries[256];
extern IDTPointer IDTPtr;

void IDTDefaultHandler();

void IDTSetEntry(U8 num, U32 base, U16 selector, U8 type_attr);
void IDTInit();