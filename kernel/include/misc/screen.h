#pragma once
#include <lib/Types.h>

typedef struct Gr {
    U32 *buffer;
    U32 width;
    U32 height;
} Gr;

U0 GrPutChar(Gr *gr, Char c, U32 x, U32 y, U32 fg, U32 bg);
U0 GrFill(Gr *gr, U32 color);
U0 GrDrawCircle(Gr *gr, U32 x, U32 y, U32 r, U32 color);
U0 GrDrawRect(Gr *gr, U32 x, U32 y, U32 w, U32 h, U32 color);
U0 GrDrawPixel(Gr *gr, U32 x, U32 y, U32 color);
U32 GrPeek(Gr *gr, U32 x, U32 y);
