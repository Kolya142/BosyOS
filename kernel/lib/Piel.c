#include <lib/Piel.h>

PielCanvas PielCanvasMake(U8* arr, U32 rwidth, U32 width, U32 height) {
    return (PielCanvas) {
        .arr = arr,
        .rwidth = rwidth,
        .width = width,
        .height = height
    };
}
U0 PielPixel(PielCanvas *cv, U32 x, U32 y, U8 c) {
    if (x >= cv->width || y >= cv->height)
        return;
    cv->arr[x + cv->rwidth * y] = c;
}
U0 PielCircle(PielCanvas *cv, U32 x, U32 y, U32 r, U8 c) {
    U32 sr = r*r;
    for (U32 i = x-r; i < x+r; ++i) {
        U32 si = (i-x)*(i-x);
        for (U32 j = y-r; j < y+r; ++j) {
            if ((si+(j-y)*(j-y)) < sr) {
                PielPixel(cv, i, j, c);
            }
        }
    }
}

static U0 PielLineLow(PielCanvas *cv, U32 x1, U32 y1, U32 x2, U32 y2, U8 c) {
    I32 dx = x2 - x1;
    I32 dy = y2 - y1;
    I32 xi = 1;
    I32 yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    if (dx < 0) {
        xi = -1;
    }
    I32 D = 2*dy - dx;
    I32 y = y1;

    for (I32 x = x1; x <= x2; ++x) {
        PielPixel(cv, x, y, c);
        if (D > 0) {
            y += yi;
            D += 2 * (dy - dx);
        }
        else {
            D = D + 2*dy;
        }
    }
}

static U0 PielLineHigh(PielCanvas *cv, U32 x1, U32 y1, U32 x2, U32 y2, U8 c) {
    I32 dx = x2 - x1;
    I32 dy = y2 - y1;
    I32 xi = 1;
    I32 yi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    if (dy < 0) {
        yi = -1;
    }
    I32 D = 2*dx - dy;
    I32 x = x1;

    for (I32 y = y1; y <= y2; ++y) {
        PielPixel(cv, x, y, c);
        if (D > 0) {
            x += xi;
            D += 2 * (dx - dy);
        }
        else {
            D = D + 2*dx;
        }
    }
}
U0 PielLine(PielCanvas *cv, U32 x1, U32 y1, U32 x2, U32 y2, U8 c) {
    if (x2 < x1) {
        U32 t;
        t = x1;
        x1 = x2;
        x2 = t;

        t = y1;
        y1 = y2;
        y2 = t;
    }
    if (ABS(y2 - y1) < ABS(x2 - x1)) {
        if (x1 > x2) PielLineLow(cv, x2, y2, x1, y1, c);
        else PielLineLow(cv, x1, y1, x2, y2, c);
    }
    else {
        if (y1 > y2) PielLineHigh(cv, x2, y2, x1, y1, c);
        else PielLineHigh(cv, x1, y1, x2, y2, c);
    }    
}

U0 PielBox(PielCanvas *cv, U32 x1, U32 y1, U32 w, U32 h, U8 c) {
    for (U32 x = x1; x < x1 + w; ++x) {
        for (U32 y = y1; y < y1 + h; ++y) {
            PielPixel(cv, x, y, c);
        }
    }
}