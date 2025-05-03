#pragma once
#include <misc/screen.h>
#include <lib/MemLib.h>
#include <lib/Types.h>

typedef struct Window {
    U8 *text_buffer;
    String title;
    U32 width;
    U32 height;
    U32 x, y;
    U32 gx, gy;
    Bool isgrapping;
    I32 z;
    Bool red;
    U8 flags;
    U0(*draw)(struct Window *win);
} Window;

U0 WindowDraw(Window *win, Gr *gr);
Window WindowInit(U0(*draw)(Window *win));
U0 WindowsUpd(Gr *gr, Window **windows, U32 wincounts);
U0 WindowPrint(Window *win, String text);
U0 WindowPrintNum(Window *win, U32 num);
String WindowGetLine(Window *win);
