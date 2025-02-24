#pragma once
#include <lib/Types.h>

typedef struct VFSNode {
    Char name[32];
    Char dir[16];
    U32 (*read)(Ptr this, Ptr buf, U32 size);
    U32 (*write)(Ptr this, Ptr buf, U32 size);
    Bool exists;
    U8 data[10];
} VFSNode;

extern VFSNode *vfs;

U0 VFSInit();
String *VFSList(String dir);
U32 VFSRead(String dir, String name, Ptr buf, U32 size);
U32 VFSWrite(String dir, String name, Ptr buf, U32 size);
U0 VFSRegister(U32 id, String dir, String name, Ptr read, Ptr write);