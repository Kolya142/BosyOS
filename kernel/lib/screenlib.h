#include <drivers/screen.h>
#include <lib/types.h>
#include <kernel/alloc.h>
#pragma once

extern int other_cursor_state;
extern U16 secondvgabuf[2000];

void PrintBinary(U8 d);
void PrintHex(U32 d);
void CursorPushMove(U8 x, U8 y);
void CursorPop();
void BufferSecondToFirst();
void BufferFirstToSecond();
void BufferFlip();