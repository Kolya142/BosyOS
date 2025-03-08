#pragma once
#include <lib/Types.h>

typedef struct Token {
    U8 type;
    String str;
    U32 num;
} Token;

Token TokenNext(String str);