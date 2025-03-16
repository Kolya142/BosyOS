#pragma once
#include <arch/x86/sys/idt.h>
#include <arch/x86/cpu/cpu.h>
#include <arch/x86/cpu/io.h>

typedef struct KBStateS {
    U8 SC;      // Scan Code
    U8 Key;     // ascii Key
    Bool Shift; // Shift key
    Bool Ctrl;  // ctrl key
    Bool Super; // super key
    Bool keys[256];
} __attribute__((packed)) KBStateS;

extern volatile KBStateS KBState;
extern U32 KBTimeout;
extern U32 KBRate;


U0 KBInit();

U8 KBSCToASCIIP(U8 code);
U8 KBKeyToRus(U8 code);