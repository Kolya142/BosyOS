#include <user/user.h>

volatile Segment UserSegment = {
    .addr = (Ptr)0,
    .length = 0xFFFFFFFF,
};