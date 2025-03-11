#pragma once
#include <lib/Types.h>

#define ABS(a) ((a) > 0) ? (a) : (-a)

typedef struct PielCanvas {
    U8 *arr;
    U32 rwidth;
    U32 width;
    U32 height;
} PielCanvas;

PielCanvas PielCanvasMake(U8* arr, U32 rwidth, U32 width, U32 height);
U0 PielPixel(PielCanvas *cv, U32 x, U32 y, U8 c);
U0 PielCircle(PielCanvas *cv, U32 x, U32 y, U32 r, U8 c);
U0 PielLine(PielCanvas *cv, U32 x1, U32 y1, U32 x2, U32 y2, U8 c);
U0 PielBox(PielCanvas *cv, U32 x1, U32 y1, U32 w, U32 h, U8 c);