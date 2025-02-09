#include <drivers/screen.h>
#include <drivers/kerboard.h>
#include <lib/screenlib.h>
#include <lib/keyboardlib.h>
#include <lib/types.h>
#include <kernel/alloc.h>
#include <kernel/clock.h>
#pragma once
struct WindowData;

typedef void (*WindowHandler)(struct WindowData*);

typedef struct WindowData {
    U8 x;
    U8 y;
    U8 w;
    U8 h;
    U32 title_shift;
    char title[15];
    U8 title_len;
    U8 custom[50];
    WindowHandler handler;
} WindowData;

WindowData *WindowNew(WindowHandler handler);
void WindowUpdate(WindowData *window);
void WindowFree(WindowData *window);