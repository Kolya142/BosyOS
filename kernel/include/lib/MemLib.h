#pragma once
#include <kernel/KHeap.h>
#include <lib/Types.h>

U0 MemCpy(Ptr dst, Ptr src, U32 count);
U0 MemSet(U0 *dest, Byte val, U32 len);

typedef struct List {
    Ptr arr;
    U32 elem;
    U32 count;
    U32 length;
} List;

typedef struct LazyList {
    U32 curr_elem;
    U32(*next)(U32 this);
} LazyList;

typedef struct FIFO {
    Ptr arr;
    U32 elem;
    U32 head;
    U32 tail;
    U32 size;
    U32 count;
} FIFO;

List ListInit(U32 elem_size);
U0 ListAppend(List *this, Ptr elem);
U0 ListDestroy(List *this);

LazyList LazyListInit(U32(*next)(U32 this), U32 init);
U32 LazyListNext(LazyList *this);
U0 LazyListDestroy(LazyList *this);

FIFO FIFOInit(U32 elem_size, U32 fifo_size);
Bool FIFOWrite(FIFO *this, Ptr elem);
Bool FIFORead(FIFO *this, Ptr elem);
U0 FIFODestroy(FIFO *this);