#pragma once
#include <lib/strings/String.h>
#include <lib/Types.h>

typedef struct ACSSpliter {
    String s;
    U32 sz;
    U32 p;
    Bool isend;
} ACSSpliter;

ACSSpliter ACSSpliterInit(String s);
U0 ACSSpliterChar(ACSSpliter *self, Char split, String buf);