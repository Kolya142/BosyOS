#pragma once
#include <lib/memory/Collects.h>
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <lib/Types.h>

#define VFS_DEV 0x40000000
#define VFS_REG 0x20000000

typedef struct VFSStat {
    U32 time;
    U32 ino;
    U32 size;
    U32 mode;
} VFSStat;

typedef struct VFSNode {
    String name;
    U32(*read)(String, Ptr, U32);
    U32(*write)(String, Ptr, U32);
    U0(*stat)(String, VFSStat*);
} VFSNode;

extern List VFS;

U0 VFSInit();
U32 VFSRead(String name, Ptr buf, U32 count);
U32 VFSWrite(String name, Ptr buf, U32 count);
U0 VFSReadDir(U0(*reader)(String, VFSStat*));
U0 VFSMount(String name, U32(*read)(String, Ptr, U32), U32(*write)(String, Ptr, U32), U0(*stat)(String, VFSStat*));