#pragma once
#include <libs/types.h>

typedef struct Segment {
    Ptr addr;
    U32 length;
} Segment;

extern volatile Segment UserSegment;