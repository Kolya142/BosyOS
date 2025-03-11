#pragma once
#include <lib/Collects.h>
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/Types.h>

typedef struct VFSStat {
    U32 time;
    U32 ino;
    U32 size;
    U32 mode;
} VFSStat;

typedef struct VFSNode {
    String name;
    U0(*read)(String, Ptr, U32);
    U0(*write)(String, Ptr, U32);
} VFSNode;

extern VFSNode *VFS;

U0 VFSInit();
U32 VFSRead(String name, Ptr buf, U32 count);
U32 VFSWrite(String name, Ptr buf, U32 count);
U0 VFSReadDir(String name, U0(*reader)(String, VFSStat*));
U0 VFSMount(U0(*read)(String, Ptr, U32), U0(*write)(String, Ptr, U32), U0(*list)(String, U0(*reader)(String, VFSStat*)));