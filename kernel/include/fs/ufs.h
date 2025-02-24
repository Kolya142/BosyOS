#pragma once
#include <lib/Types.h>

typedef struct UFSNode {
    Char root[16];
    U32 (*read)(Ptr this, String dir, String name, Ptr buf, U32 size);
    U32 (*write)(Ptr this, String dir, String name, Ptr buf, U32 size);
} UFSNode;

extern UFSNode *ufs;

U32 UFSRead(String dir, String name, Ptr buf, U32 size);
U32 UFSWrite(String dir, String name, Ptr buf, U32 size);
U0 UFSRegister(Ptr read, Ptr write, String root);
U0 UFSInit();