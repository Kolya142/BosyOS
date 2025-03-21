#pragma once
#include <lib/memory/Collects.h>
#include <lib/strings/String.h>
#include <lib/memory/MemLib.h>
#include <lib/Types.h>

#define VFS_IFDEV 0x40000000
#define VFS_IFREG 0x20000000
#define VFS_IFDIR 0x10000000
#define VFS_UREAD 0x10000000
#define VFS_UWRIT 0x08000000
#define VFS_UEXEC 0x04000000
#define VFS_OREAD 0x02000000
#define VFS_OWRIT 0x01000000
#define VFS_OEXEC 0x00800000

#define VFS_MAX_OPEN 128

typedef struct VFSStat {
    U32 time;
    U32 ino;
    U32 size;
    U32 mode;
} VFSStat;

typedef struct VFSNode {
    String name;
    U32(*read)(String, Ptr, U32, U32);
    U32(*write)(String, Ptr, U32, U32);
    U0(*stat)(String, VFSStat*);
    struct VFSNode *parent;
    List child;
} VFSNode;

typedef struct VFSFD {
    VFSNode *pos;
    U32 head;
} VFSFD;

extern VFSNode *VFSRoot;

U0 VFSPathParse(String path, Char *parsd, U32 count, U32 size);
VFSNode *VFSFind(VFSNode *parent, String path);
U0 VFSInit();
U32 VFSRead(String name, Ptr buf, U32 offset, U32 count);
U32 VFSWrite(String name, Ptr buf, U32 offset, U32 count);
U32 VFSOpen(String filename);
U0 VFSClose(U32 fd);
U32 VFSWriteV(U32 fd, Ptr buf, U32 count);
U32 VFSReadV(U32 fd, Ptr buf, U32 count);
U0 VFSLStat(String filename, VFSStat *stat);
U0 VFSLSeek(U32 fd, U32 off, U32 whence);
U0 VFSReadDir(String path, U0(*reader)(String, VFSStat*));
U0 VFSMount(String name, U32(*read)(String, Ptr, U32, U32), U32(*write)(String, Ptr, U32, U32), U0(*stat)(String, VFSStat*));
U0 VFSDirMk(String name);