#pragma once
#include <arch/i386/cpu.h>
#include <arch/i386/idt.h>

typedef struct KBStateS {
    U8 SC;
    Char Key;
    Bool Shift;
    Bool Ctrl;
    Bool Super;
    Bool Keys[256];
} KBStateS;

extern volatile KBStateS KBState;
extern U32 KBTimeout;
extern U32 KBRate;

U0 KBInit();

U8 KBSCToASCIIP(U8 code);