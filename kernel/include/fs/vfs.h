#pragma once
#include <lib/memory/Collects.h>
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <lib/Types.h>

#define VFS_DEV 0x40000000
#define VFS_REG 0x20000000
#define VFS_UREAD 0x10000000
#define VFS_UWRIT 0x08000000
#define VFS_UEXEC 0x04000000
#define VFS_OREAD 0x02000000
#define VFS_OWRIT 0x01000000
#define VFS_OEXEC 0x00800000

typedef struct VFSStat {
    U32 time;
    U32 ino;
    U32 size;
    U32 mode;
} VFSStat;

typedef struct VFSFD {
    U32 pos;
    U32 head;
} VFSFD;

typedef struct VFSNode {
    String name;
    U32(*read)(String, Ptr, U32, U32);
    U32(*write)(String, Ptr, U32, U32);
    U0(*stat)(String, VFSStat*);
} VFSNode;

extern List VFS;

U0 VFSInit();
U32 VFSRead(String name, Ptr buf, U32 offset, U32 count);
U32 VFSWrite(String name, Ptr buf, U32 offset, U32 count);
U32 VFSOpen(String filename);
U0 VFSClose(U32 fd);
U32 VFSWriteV(U32 fd, Ptr buf, U32 count);
U32 VFSReadV(U32 fd, Ptr buf, U32 count);
U32 VFSLStat(String filename, VFSStat *stat);
U0 VFSReadDir(U0(*reader)(String, VFSStat*));
U0 VFSMount(String name, U32(*read)(String, Ptr, U32, U32), U32(*write)(String, Ptr, U32, U32), U0(*stat)(String, VFSStat*));