#pragma once
#include <arch/cpu.h>
#include <arch/io.h>

typedef struct KBStateS {
    U8 SC;
    U8 Key;
    Bool Shift;
    Bool Ctrl;
} __attribute__((packed)) KBStateS;

extern volatile KBStateS KBState;

U0 KBInit();

U8 KBSCToASCIIP(U8 code);