#pragma once
#include <arch/cpu.h>
#include <arch/io.h>
#define PIT_FREQ 1193182

extern volatile U32 PITTime;

U0 PITInit();