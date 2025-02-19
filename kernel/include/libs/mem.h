#pragma once
#include <libs/types.h>

Ptr MAlloc(U32 size);
U0 MFree(Ptr ptr);
Ptr MReAlloc(Ptr ptr, U32 olds, U32 news);
Ptr MCAlloc(U32 elem, U32 count);