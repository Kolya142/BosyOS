#pragma once
#include <lib/Types.h>

#define TOK_NUMBER 1
#define TOK_WORD 2
#define TOK_SPEC 3
#define TOK_STR  4

typedef struct Token {
    U8 type;
    Char str[32];
} Token;

U32 TokenNext(String str, Token *tok);