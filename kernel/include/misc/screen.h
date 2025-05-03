#pragma once
#include <lib/Types.h>

typedef struct Gr {
    U32 *buffer;
    U32 width;
    U32 height;
} Gr;

U0 GrPutChar(Gr *gr, Char c, I32 x, I32 y, U32 fg, U32 bg);
U0 GrDrawLine(Gr *gr, I32 x0, I32 y0, I32 x1, I32 y1, U32 color);
U0 GrFill(Gr *gr, U32 color);
U0 GrDrawCircle(Gr *gr, I32 x, I32 y, U32 r, U32 color);
U0 GrDrawRect(Gr *gr, I32 x, I32 y, I32 w, I32 h, U32 color);
U0 GrDrawPixel(Gr *gr, U32 x, U32 y, U32 color);
U32 GrPeek(Gr *gr, U32 x, U32 y);
