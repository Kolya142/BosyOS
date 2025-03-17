#pragma once
#include <lib/memory/MemLib.h>
#include <drivers/pty.h>
#include <lib/ASCIIP.h>
#include <lib/Types.h>

typedef struct TTY {
    PTY *pty;
    U0(*flush)(struct TTY *this, U32 ttyid);
    U8 last[2048];
    U32 last_index;
    U8 data[20];
} TTY;

extern List PTYs;
extern List TTYs;
extern U32 TTYCurrent;

U0 TTYInit();
U32 PTYNew(U32 size, U32 width, U32 height);
U32 TTYNew(U0(*flush)(TTY *this, U32 ttyid), U32 pty);
U32 TTYWrite(U32 tty, U32 pd, String content, U32 count);
U32 TTYRead(U32 tty, U32 pd, String content, U32 count);
U0 TTYFlush(U32 tty);
U0 TTYSwitch(U32 id);
U0 TTYInput();

U0 TTYRenderT(TTY *this, U32 ttyid);
U0 TTYRenderS(TTY *this, U32 ttyid);
U0 TTYRenderG(TTY *this, U32 ttyid);