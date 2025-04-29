#pragma once
#include <arch/i386/cpu.h>
#include <arch/i386/idt.h>

extern volatile U32 PITTime;

U0 PITInit();