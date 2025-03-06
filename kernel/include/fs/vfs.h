#pragma once
#include <lib/Collects.h>
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/Types.h>

typedef struct VFSNode {
    String name;
    U0(*read)(String, Ptr, U32);
    U0(*write)(String, Ptr, U32);
    U0(*list)(struct VFSNode*, U32);
    struct VFSNode *parent;
    struct List children;
} VFSNode;

extern VFSNode VFSRoot;

U0 VFSInit();
List VFSParsePath(const String path);
U32 VFSRead(String name, Ptr buf, U32 count);
U32 VFSWrite(String name, Ptr buf, U32 count);
U0 VFSList(String name, VFSNode *buf, U32 count);
U0 VFSMount(String path, U0(*read)(String, Ptr, U32), U0(*write)(String, Ptr, U32), U0(*list)(struct VFSNode*, U32));