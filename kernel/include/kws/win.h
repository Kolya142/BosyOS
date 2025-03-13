#pragma once
#include <lib/graphics/Graphics.h>
#include <kws/input.h>
#include <lib/graphics/Piel.h>

#define WIN_UNMOVEBLE 1
#define WIN_UNCLOSABLE 2

typedef struct Win
{
    Char title[16];
    U32 x;
    U32 y;
    U32 w;
    U32 h;
    U16 flags;
    PielCanvas canvas;
    U0(*update)(Ptr this);
    KWSInputData inp;
} Win;

extern Win windows[64];

U0 WPrintF(Win *win, U32 x, U32 y, String format, ...);

U32 WinSpawn(Win *win);
U0 WindowsUpdate();

U0 WinPrint(U32 x, U32 y, String text);
Win WinMake(U32 x, U32 y, U32 w, U32 h, String title, U16 flags);
U0 WinDraw(Win *win);