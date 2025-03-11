#pragma once
#include <lib/graphics/Graphics.h>
#include <kws/input.h>
#include <lib/graphics/Piel.h>

typedef struct Win
{
    Char title[16];
    U32 x;
    U32 y;
    U32 w;
    U32 h;
    PielCanvas canvas;
} Win;

extern Win windows[64];

U0 WinPrint(U32 x, U32 y, String text);
Win WinMake(U32 x, U32 y, U32 w, U32 h, String title);
U0 WinDraw(Win *win);