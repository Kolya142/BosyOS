#include <lib/IO/PTY.h>

PTerm PTermInit(U32 size) {
    PTerm term;
    term.in = FIFOInit(1, size);
    term.out = FIFOInit(1, size);
    term.err = FIFOInit(1, size);
    return term;
}

U32 PTermWrite(PTerm *term, U8 pd, Ptr buf, U32 count) {
    FIFO *fifo;
    switch (pd)
    {
    case 0:
        fifo = &term->in;
        break;
    case 1:
        fifo = &term->out;
        break;
    case 2:
        fifo = &term->err;
        break;
    default:
        return 0;
    }
    U32 rcount = 0;
    for (U32 i = 0; i < count; ++i) {
        if (FIFOWrite(fifo, &buf[i])) {
            ++rcount;
        }
        else {
            break;
        }
    }
    return rcount;
}
U32 PTermRead(PTerm *term, U8 pd, Ptr buf, U32 count) {
    FIFO *fifo;
    switch (pd)
    {
    case 0:
        fifo = &term->in;
        break;
    case 1:
        fifo = &term->out;
        break;
    case 2:
        fifo = &term->err;
        break;
    default:
        return 0;
    }
    U32 rcount = 0;
    for (U32 i = 0; i < count; ++i) {
        if (FIFORead(fifo, &buf[i])) {
            ++rcount;
        }
        else {
            break;
        }
    }
    return rcount;
}