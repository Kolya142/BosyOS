#pragma once
#include <lib/Types.h>
#include <arch/idt.h>
#include <arch/io.h>

extern I32 MouseX, MouseY;
extern U8 MouseBtn;

U0 MouseInit();