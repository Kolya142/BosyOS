#pragma once
#include <libs/types.h>

typedef struct GDTEntry {
    U16 lim_low;
    U16 base_low;
    U8 base_middle;
    U8 access;
    U8 length;
    U8 base_high;
} __attribute__((packed)) GDTEntry;

typedef struct GDTPointer {
    U16 size;
    U32 addr;
} __attribute__((packed)) GDTPointer;

typedef struct TSSt {
    U32 prev;
    U32 esp0;
    U32 ss0;
    U32 unused[23];
} __attribute__((packed)) TTSt;

extern TTSt TSS;

extern GDTEntry GDTEntries[];
extern GDTPointer GDTPtr;

U0 GDTInit();
U0 GDTLoad();
U0 GDTSet(U32 base, U32 limit);
U0 GDTEntrySet(U8 index, U32 base, U32 limit, U8 access, U8 len);
U0 SYSUserSetup(Ptr addr, Ptr stack);