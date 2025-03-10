#pragma once
#include <drivers/ps2.h>
#include <lib/Types.h>
#include <arch/idt.h>
#include <arch/io.h>

extern I32 MouseVX, MouseVY;
extern I32 MouseX, MouseY;
extern U8 MouseBtn;

U0 MouseUpdate();
U0 MouseInit();