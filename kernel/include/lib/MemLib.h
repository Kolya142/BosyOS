#pragma once
#include <lib/Types.h>

Ptr MAlloc(U32 size);
U0 MFree(Ptr ptr);
Ptr MReAlloc(Ptr ptr, U32 olds, U32 news);
Ptr MCAlloc(U32 elem, U32 count);
U0 *MemCpy(U0 *destp, const U0 *src, U32 len);
U0 MemSet(U0 *dest, Byte val, U32 len);