#pragma once
#include <lang/Parser.h>
#include <lib/Window.h>

typedef struct Object {
    U32 type;
    union
    {
        String str;
        List list;
        U32 num;
    } obj;
    
} Object;

Object Interpretter(Window *win, Gr *gr, Ast ast);