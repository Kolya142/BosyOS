#include <lib/time/Time.h>
#include <lib/memory/MemLib.h>
#include <fs/vfs.h>

List VFS;

U0 VFSInit() {
    VFS = ListInit(sizeof(VFSNode));
}
U32 VFSRead(String name, Ptr buf, U32 offset, U32 count) {
    for (U32 i = 0; i < VFS.count; ++i) {
        if (!StrCmp(((VFSNode*)VFS.arr)[i].name, name)) {
            return ((VFSNode*)VFS.arr)[i].read(name, buf, offset, count);
        }
    }
    return 0;
}
U32 VFSWrite(String name, Ptr buf, U32 offset, U32 count) {
    for (U32 i = 0; i < VFS.count; ++i) {
        if (!StrCmp(((VFSNode*)VFS.arr)[i].name, name)) {
            return ((VFSNode*)VFS.arr)[i].write(name, buf, offset, count);
        }
    }
    return 0;
}
U32 VFSOpen(String filename) {
    for (U32 i = 0; i < VFS.count; ++i) {
        if (!StrCmp(((VFSNode*)VFS.arr)[i].name, filename)) {
            VFSFD *fd = MAlloc(sizeof(VFSFD));
            fd->pos = i;
            fd->head = 0;
            return (U32)fd;
        }
    }
    return 0xFFFFFFFF;
}
U0 VFSClose(U32 fd) {
    if (!fd) {
        return;
    }
    MFree((Ptr)fd);
}
U32 VFSWriteV(U32 fd, Ptr buf, U32 count) {
    VFSFD *file = (VFSFD*)fd;
    VFSNode *node = &((VFSNode*)VFS.arr)[file->pos];
    U32 r = VFSWrite(node->name, buf, file->head, count);
    file->head += r;
    return r;
}
U32 VFSReadV(U32 fd, Ptr buf, U32 count) {
    VFSFD *file = (VFSFD*)fd;
    VFSNode *node = &((VFSNode*)VFS.arr)[file->pos];
    U32 r = VFSRead(node->name, buf, file->head, count);
    file->head += r;
    return r;
}

U32 VFSLStat(String filename, VFSStat *stat) {
    for (U32 i = 0; i < VFS.count; ++i) {
        VFSNode *node = &((VFSNode*)VFS.arr)[i];
        if (!StrCmp(node->name, filename)) {
            node->stat(filename, stat);
            return 0;
        }
    }
    return 1;
}

U0 VFSReadDir(U0(*reader)(String, VFSStat*)) {
    for (U32 i = 0; i < VFS.count; ++i) {
        VFSNode *node = &((VFSNode*)VFS.arr)[i];
        if (node->stat) {
            VFSStat stat;
            node->stat(node->name, &stat);
            reader(node->name, &stat);
        }
    }
}
U0 VFSMount(String name, U32(*read)(String, Ptr, U32, U32), U32(*write)(String, Ptr, U32, U32), U0(*stat)(String, VFSStat*)) {
    for (U32 i = 0; i < VFS.count; ++i) {
        VFSNode *node = &((VFSNode*)VFS.arr)[i];
        if (!StrCmp(node->name, name)) {
            return;
        }
    }
    VFSNode node = {
        .name = name,
        .read = read,
        .write = write,
        .stat = stat
    };
    ListAppend(&VFS, &node);
}