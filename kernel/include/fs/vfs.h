#pragma once
#include <lib/Collects.h>
#include <lib/String.h>
#include <lib/MemLib.h>
#include <lib/Types.h>

#define VFS_DIR  0x80000000
#define VFS_REG  0x40000000
#define VFS_READ 0x20000000
#define VFS_WRIT 0x10000000
#define VFS_EXEC 0x08000000

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
    U0(*list)(String, U0(*reader)(String, VFSStat*));
    struct VFSNode *parent;
    struct List children;
} VFSNode;

extern VFSNode VFSRoot;

U0 VFSInit();
List VFSParsePath(const String path);
U32 VFSRead(String name, Ptr buf, U32 count);
U32 VFSWrite(String name, Ptr buf, U32 count);
U0 VFSReadDir(String name, U0(*reader)(String, VFSStat*));
U0 VFSMount(String path, U0(*read)(String, Ptr, U32), U0(*write)(String, Ptr, U32), U0(*list)(String, U0(*reader)(String, VFSStat*)));