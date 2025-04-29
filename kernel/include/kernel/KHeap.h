#pragma once
#include <lib/Types.h>

#define KERNEL_HEAP_START 0x01300000
#define KERNEL_HEAP_END   0x10000000

U0 KHeapInit();

Bool KHeapSanityCheck();

Ptr KMAlloc(U32 count);
U0 KMFree(Ptr ptr);