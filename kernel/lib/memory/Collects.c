#include <lib/memory/Collects.h>
List ListInit(U32 elem_size) {
    List list;
    list.arr = MCAlloc(elem_size, 8);
    list.elem = elem_size;
    list.count = 0;
    list.length = 8;
    return list;
}
U0 ListAppend(List *this, Ptr elem) {
    if (this->count == this->length) {
        this->arr = MReAlloc(this->arr, this->length * this->elem, (this->length << 1) * this->elem);
        this->length <<= 1;
    }
    MemCpy(this->arr + this->count * this->elem, elem, this->elem);
    ++this->count;
}
U0 ListDestroy(List *this) {
    MFree(this->arr);
    this->arr = 0;
    this->count = 0;
    this->elem = 0;
    this->length = 0;
}

LazyList LazyListInit(U32(*next)(U32 this), U32 init) {
    LazyList list;
    list.curr_elem = init;
    list.next = next;
    return list;
}
U32 LazyListNext(LazyList *this) {
    U32 prev = this->curr_elem;
    this->curr_elem = this->next(this->curr_elem);
    return prev;
}
U0 LazyListDestroy(LazyList *this) {
    this->curr_elem = 0;
    this->next = (U32(*)(U32))0;
}

FIFO FIFOInit(U32 elem_size, U32 fifo_size) {
    FIFO fifo;
    fifo.arr = MCAlloc(elem_size, fifo_size);
    fifo.head = 0;
    fifo.tail = 0;
    fifo.elem = elem_size;
    fifo.size = fifo_size;
    fifo.count = 0;
    return fifo;
}
Bool FIFOWrite(FIFO *this, Ptr elem) {
    if (this->count >= this->size) {
        return False;
    }
    MemCpy(this->arr + this->head * this->elem, elem, this->elem);
    this->head = (this->head + 1) % this->size;
    ++this->count;
    return True;
}
Bool FIFORead(FIFO *this, Ptr elem) {
    if (this->count == 0) {
        return False;
    }
    MemCpy(elem, this->arr + this->tail * this->elem, this->elem);
    this->tail = (this->tail + 1) % this->size;
    --this->count;
    return True;
}
U0 FIFODestroy(FIFO *this) {
    MFree(this->arr);
    this->arr = 0;
    this->count = 0;
    this->elem = 0;
    this->head = 0;
    this->size = 0;
    this->tail = 0;
}