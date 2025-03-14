#include <lib/time/Time.h>
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
U0 VFSReadDir(U0(*reader)(String, VFSStat*)) {
    for (U32 i = 0; i < VFS.count; ++i) {
        VFSNode *node = &((VFSNode*)VFS.arr)[i];
        VFSStat stat;
        node->stat(node->name, &stat);
        reader(node->name, &stat);
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