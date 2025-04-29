#pragma once
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
    List child;
    U0(*read)(String path, String name, Ptr buf, U32 count);
    U0(*write)(String path, String name, Ptr buf, U32 count);
    U0(*mkdir)(String path, String name);
    U0(*create)(String path, String name);
    U0(*remove)(String path, String name);
} VFSDrive;
