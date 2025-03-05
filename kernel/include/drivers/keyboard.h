#pragma once
#include <arch/idt.h>
#include <arch/cpu.h>
#include <arch/io.h>

typedef struct KBStateS {
    U8 SC;      // Scan Code
    U8 Key;     // ascii Key
    Bool Shift; // Shift key
    Bool Ctrl;  // ctrl key
    Bool Super; // super key
    Bool keys[256];
} __attribute__((packed)) KBStateS;

extern volatile KBStateS KBState;

U0 KBInit();

U8 KBSCToASCIIP(U8 code);
U8 KBKeyToRus(U8 code);