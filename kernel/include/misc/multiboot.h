#pragma once
#include <lib/Types.h>

struct MultiBoot {
	U32 total;
	U32 reservd;
} __attribute__((packed));

struct MultiBootTag {
	U32 type;
	U32 size;
} __attribute__((packed));

extern struct MultiBoot *mb;