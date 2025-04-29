#pragma once
#include <drivers/controllers/ps2.h>
#include <lib/Types.h>

extern I32 MouseVX, MouseVY;
extern I32 MouseX, MouseY;
extern U8 MouseBtn;

U0 MouseUpdate();
U0 MouseInit();