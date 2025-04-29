#include <misc/screen.h>
#include <lib/MemLib.h>

FrameBuffer fb0;
extern unsigned char TTYFont[256][8];

U0 GrPutChar(Gr *gr, Char c, U32 x, U32 y, U32 fg, U32 bg) {
    for (U32 i = 0; i < 8; ++i) {
        if (x + i > gr->width) continue;
        for (U32 j = 0; j < 8; ++j) {
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
U0 GrDrawRect(Gr *gr, U32 x, U32 y, U32 w, U32 h, U32 color) {
    for (U32 i = 0; i < w; ++i) {
        for (U32 j = 0; j < h; ++j) {
	    gr->buffer[gr->width * (y + j) + x + i] = color;
	}
    }
}
U0 GrDrawPixel(Gr *gr, U32 x, U32 y, U32 color) {
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
U0 GrDrawCircle(Gr *gr, U32 x, U32 y, U32 r, U32 color) {
    U32 R = r * r;
    U32 d = r * 2;
    for (U32 i = x; i < x + d; ++i) {
        I32 i0 = i - x - r;
        for (U32 j = y; j < y + d; ++j) {
            I32 j0 = j - y - r;
            if (i <= gr->width && j <= gr->height && (i0*i0+j0*j0) <= R) {
                gr->buffer[i+j*gr->width] = color;
            }
        }
    }
}
