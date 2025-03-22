#pragma once
#include <drivers/controllers/ps2.h>
#include <lib/Types.h>
#include <arch/x86/sys/idt.h>
#include <arch/x86/cpu/io.h>

extern I32 MouseVX, MouseVY;
extern I32 MouseX, MouseY;
extern U8 MouseBtn;

U0 MouseUpdate();
U0 MouseInit();