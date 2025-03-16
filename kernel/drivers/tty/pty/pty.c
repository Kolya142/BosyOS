#include <drivers/pty.h>


U32 PTYWrite(PTY *pty, U32 pd, String content, U32 count) {
    FIFO *fifo;
    switch (pd) {
        case 0:
            fifo = &pty->in;
            break;
        case 1:
            fifo = &pty->out;
            break;
        case 2:
            fifo = &pty->err;
            break;
    }
    U32 i;
    for (i = 0; i < count; ++i) {
        if (!FIFOWrite(fifo, &content[i])) {
            break;
        }
    }
    return i;
}
U32 PTYRead(PTY *pty, U32 pd, String content, U32 count) {
    FIFO *fifo;
    switch (pd) {
        case 0:
            fifo = &pty->in;
            break;
        case 1:
            fifo = &pty->out;
            break;
        case 2:
            fifo = &pty->err;
            break;
    }
    U32 i;
    for (i = 0; i < count; ++i) {
        if (!FIFORead(fifo, &content[i])) {
            break;
        }
    }
    return i;
}
U32 PTYMake(PTY *pty, U32 size, U32 width, U32 height) {
    pty->cursor = 0;
    pty->in = FIFOInit(1, size);
    pty->out = FIFOInit(1, size);
    pty->err = FIFOInit(1, size);
    pty->width = width;
    pty->height = height;
}