#pragma once
#include <lang/Tokenizer.h>
#include <lib/MemLib.h>

typedef struct Ast {
    Bool is_list;
    Token tok;
    List elements;
} Ast;

Ast Parse(String *code);