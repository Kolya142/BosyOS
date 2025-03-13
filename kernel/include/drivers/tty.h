#pragma once
#include <lib/ASCIIP.h>
#include <lib/Types.h>
#include <drivers/video/vga.h>
#include <drivers/serial/serial.h>
#include <lib/graphics/Graphics.h>
#include <drivers/input/keyboard.h>
#include <drivers/sys/beep.h>
#include <lib/IO/PTY.h>

U0 TTYRenderGS();
U0 TTYRenderG();
U0 TTYRenderT();
U0 TTYRenderS();
U0 TTYInput();

typedef enum {
    TTYC_VGA,
    TTYC_320,
    TTYC_SER,
    TTYC_RES
} TTYContext;

typedef struct TTY
{
    U0(*render)();
    U32 width;
    U32 height;
} TTY;

extern U32 TTYCursor;
extern PTerm *VTerm;
extern PTerm VTerms[4];
extern U32 TTermID;
extern U8 TTYBuffers[4][8192];
extern U32 TTYBuffersIndex[4];
extern TTY TTerm;
extern unsigned char TTYFont[256][5];
extern U32 TTYGSX;
extern U32 TTYGSY;
extern Bool TTYCanonical;