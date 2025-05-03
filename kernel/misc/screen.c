#include <misc/screen.h>
#include <lib/MemLib.h>

FrameBuffer fb0;
extern unsigned char TTYFont[256][8];

U0 GrPutChar(Gr *gr, Char c, I32 x, I32 y, U32 fg, U32 bg) {
    for (I32 i = 0; i < 8; ++i) {
        if (x + i > gr->width) continue;
        for (I32 j = 0; j < 8; ++j) {
            if (y + j > gr->height) continue;
            if (TTYFont[c][j] & (1 << (7 - i))) {
                gr->buffer[gr->width * (y + j) + x + i] = fg;
            }
            else {
                gr->buffer[gr->width * (y + j) + x + i] = bg;
            }
        }
    }
}
U0 GrDrawRect(Gr *gr, I32 x, I32 y, I32 w, I32 h, U32 color) {
    for (I32 i = 0; i < w; ++i) {
        if (x + i > gr->width) continue;
        for (I32 j = 0; j < h; ++j) {
            if (y + j > gr->height) continue;
	    gr->buffer[gr->width * (y + j) + x + i] = color;
	}
    }
}
static I32 abs(I32 x) {
    return x < 0 ? -x : x;
}
U0 GrDrawLine(Gr *gr, I32 x0, I32 y0, I32 x1, I32 y1, U32 color) {
    I32 dx = abs(x1 - x0);
    I32 dy = -abs(y1 - y0);
    I32 sx = x0 < x1 ? 1 : -1;
    I32 sy = y0 < y1 ? 1 : -1;
    I32 y = y0;
    I32 err = dx + dy;
    for (;;) {
	GrDrawPixel(gr, x0, y0, color);
        I32 e2 = 2 * err;
	if (e2 >= dy) {
	    if (x0 == x1) break;
	    err += dy;
	    x0 += sx;
	}
	if (e2 <= dx) {
	    if (y0 == y1) break;
	    err += dx;
	    y0 += sy;
	}
    }
}
U0 GrDrawPixel(Gr *gr, U32 x, U32 y, U32 color) {
   if (gr->width * y + x > gr->width*gr->height) return;
   gr->buffer[gr->width * y + x] = color;
}
U32 GrPeek(Gr *gr, U32 x, U32 y) {
   return gr->buffer[gr->width * y + x];
}
U0 GrFill(Gr *gr, U32 color) {
    U32 s = gr->width * gr->height;
    for (U32 i = 0; i < s; ++i) {
        gr->buffer[i] = color;
    }
}
U0 GrDrawCircle(Gr *gr, I32 x, I32 y, U32 r, U32 color) {
    U32 R = r * r;
    U32 d = r * 2;
    for (I32 i = x; i < x + d; ++i) {
        I32 i0 = i - x - r;
        for (I32 j = y; j < y + d; ++j) {
            I32 j0 = j - y - r;
            if (i <= gr->width && j <= gr->height && (i0*i0+j0*j0) <= R) {
                gr->buffer[i+j*gr->width] = color;
            }
        }
    }
}
