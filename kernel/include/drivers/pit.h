#pragma once
#include <arch/idt.h>
#include <arch/io.h>
#define PIT_FREQ 1193182

extern volatile U32 PITTime;
extern volatile U32 BosyTime;

#define BOSY_EPOCH (1735689600UL)

U0 PITInit();