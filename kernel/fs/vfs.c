#include <lib/time/Time.h>
#include <lib/memory/MemLib.h>
#include <lib/IO/TTY.h>
#include <fs/vfs.h>

VFSNode *VFSRoot;
static VFSFD fds[VFS_MAX_OPEN];

U0 VFSPathParse(String path, Char *parsd, U32 count, U32 size) {
    U32 i = 0;
    U32 j = 0;

    while (*path == '/') {
        ++path;
    }

    while (*path) {
        if (*path == '/') {
            parsd[i * size + j] = 0;
            ++i;
            if (i >= count) {
                break;
            }
            j = 0;
        }
        else {
            if (j < size - 1) {
                parsd[i * size + j] = *path;
                ++j;
            }
        }
        ++path;
    }
    if (j) parsd[i * size + j] = 0;
    if (i < count - 1) parsd[(i + 1) * size] = 0;
}
VFSNode *VFSFind(VFSNode *parent, String path) {
    // SerialPrintF("Path: %s\n", path);
    Char parsd[10][64] = {0};
    VFSPathParse(path, (Char*)parsd, 10, 64);
    U32 depth = 0;
    VFSNode *curr = parent;
    while (parsd[depth][0]) {
        VFSNode *child = Null;
        for (U32 i = 0; i < curr->child.count; ++i) {
            child = &((VFSNode*)curr->child.arr)[i];
            if (!StrCmp(child->name, parsd[depth])) {
                curr = child;
                break;
            }
        }
        // SerialPrintF("Curr: %s", curr->name);
        ++depth;
        if (!child) break;
    }
    // SerialPrintF("Node: %s", curr->name);
    return curr;
}
U0 VFSInit() {
    VFSRoot = MAlloc(sizeof(VFSNode));
    VFSRoot->stat = Null;
    VFSRoot->read = Null;
    VFSRoot->write = Null;
    VFSRoot->parent = NULL;
    VFSRoot->name = MAlloc(2);
    MemCpy(VFSRoot->name, "/", 2);
    VFSRoot->child = ListInit(sizeof(VFSNode));
}
U32 VFSRead(String name, Ptr buf, U32 offset, U32 count) {
    VFSNode *node = VFSFind(VFSRoot, name);
    if (!node || !node->read) {
        return 0;
    }
    return node->read(name, buf, offset, count);
}

U32 VFSWrite(String name, Ptr buf, U32 offset, U32 count) {
    VFSNode *node = VFSFind(VFSRoot, name);
    if (!node || !node->write) {
        return 0;
    }

    return node->write(name, buf, offset, count);
}
U32 VFSOpen(String filename) {
    Char parsd[10][64];
    VFSPathParse(filename, (Char*)parsd, 10, 64);
    U32 i = 0;
    for (;i < 10;) {
        if (!parsd[i][0]) {
            break;
        }
        ++i;
    }
    if (i) {
        --i;
    }
    VFSNode *node = VFSFind(VFSRoot, filename);
    if (!node || StrCmp(node->name, parsd[i])) {
        return -1;
    }

    for (U32 i = 4; i < VFS_MAX_OPEN; ++i) {
        if (!fds[i].pos) {
            fds[i].pos = node;
            U32 s = StrLen(filename) + 1;
            fds[i].name = MAlloc(s);
            MemCpy(fds[i].name, filename, s);
            fds[i].head = 0;
            return i;
        }
    }
    return -1;
}
U0 VFSClose(U32 fd) {
    if (fd >= VFS_MAX_OPEN || !fds[fd].pos) {
        return;
    }
    MFree(fds[fd].name);
    fds[fd].pos = 0;
}
U32 VFSWriteV(U32 fd, Ptr buf, U32 count) {
    if (fd >= VFS_MAX_OPEN || !fds[fd].pos) {
        return 0;
    }
    if (fds[fd].pos->stat) {
        VFSStat stat;
        fds[fd].pos->stat(fds[fd].name, &stat);
        if (!(stat.mode & VFS_UWRIT)) {
            return 0;
        }
    }
    if (!fds[fd].pos->write) {
        return 0;
    }
    U32 a = fds[fd].pos->write(fds[fd].name, buf, fds[fd].head, count);
    fds[fd].head += a;
    return a;
}
U32 VFSReadV(U32 fd, Ptr buf, U32 count) {
    if (fd >= VFS_MAX_OPEN || !fds[fd].pos) {
        return 0;
    }
    if (fds[fd].pos->stat) {
        VFSStat stat;
        fds[fd].pos->stat(fds[fd].name, &stat);
        if (!(stat.mode & VFS_UREAD)) {
            return 0;
        }
    }
    if (!fds[fd].pos->read) {
        return 0;
    }
    U32 a = fds[fd].pos->read(fds[fd].name, buf, fds[fd].head, count);
    // PrintF("reading %s %d\n", fds[fd].name, a);
    fds[fd].head += a;
    return a;
}
U0 VFSLStat(String filename, VFSStat *stat) {
    VFSNode *node = VFSFind(VFSRoot, filename);
    if (!node || !node->stat) {
        return;
    }

    return node->stat(filename, stat);
}
U0 VFSLSeek(U32 fd, U32 off, U32 whence) {
    switch (whence) {
        case 0:
            fds[fd].head = off;
            break;
        case 1:
            fds[fd].head += off;
            break;
        case 2:
            // 2 whence is stange
            break;
    }
}
U0 VFSReadDir(String path, U0(*reader)(String, VFSStat*)) {
    VFSNode *dir = VFSFind(VFSRoot, path);
    if (!dir) {
        return;
    }
    for (U32 i = 0; i < dir->child.count; ++i) {
        VFSNode *curr = &((VFSNode*)dir->child.arr)[i];
        VFSStat stat;
        if (curr->stat) {
            curr->stat(curr->name, &stat);
        }
        else {
            MemSet(&stat, 0, sizeof(VFSStat));
            stat.mode = VFS_IFDIR;
        }
        reader(curr->name, &stat);
    }
}
U0 VFSMount(String name, U32(*read)(String, Ptr, U32, U32), U32(*write)(String, Ptr, U32, U32), U0(*stat)(String, VFSStat*)) {
    Char parsd[10][64];
    VFSPathParse(name, (Char*)parsd, 10, 64);
    U32 i = 0;
    for (;i < 10;) {
        if (!parsd[i][0]) {
            break;
        }
        ++i;
    }
    if (i) {
        --i;
    }

    VFSNode *dir = VFSFind(VFSRoot, name);
    // SerialPrintF("Parent Node: %s", dir->name);
    if (!dir || !StrCmp(dir->name, parsd[i])) {
        return;
    }

    VFSNode *new_node = (VFSNode*)MAlloc(sizeof(VFSNode));
    new_node->read = read;
    new_node->write = write;
    new_node->stat = stat;
    new_node->parent = dir;
    U32 s = StrLen(parsd[i]);
    new_node->name = MAlloc(s+1);
    new_node->name[s] = 0;
    MemCpy(new_node->name, parsd[i], s);

    ListAppend(&dir->child, new_node);
    for (U32 i = 0; i < dir->child.count; ++i) {
        VFSNode *child = &((VFSNode*)dir->child.arr)[i];
        // SerialPrintF("-> ()%s", child->name);
    }
}
U0 VFSDirMk(String name, U0(*create)(String)) {
    Char parsd[10][64];
    VFSPathParse(name, (Char*)parsd, 10, 64);
    U32 i = 0;
    for (;i < 10;) {
        if (!parsd[i][0]) {
            break;
        }
        ++i;
    }
    if (i) {
        --i;
    }

    VFSNode *parent = VFSFind(VFSRoot, name);
    if (!parent || !StrCmp(parent->name, parsd[i])) {
        return;
    }
    VFSNode *node = (VFSNode*)MAlloc(sizeof(VFSNode));
    U32 s = StrLen(parsd[i]);
    node->name = MAlloc(s+1);
    node->name[s] = 0;
    MemCpy(node->name, parsd[i], s);
    node->read = Null;
    node->write = Null;
    node->create = create;
    node->stat = Null;
    node->child = ListInit(sizeof(VFSNode));

    ListAppend(&parent->child, node);
}
U0 VFSCreate(String name) {
    Char parsd[10][64];
    VFSPathParse(name, (Char*)parsd, 10, 64);
    U32 i = 0;
    for (;i < 10;) {
        if (!parsd[i][0]) {
            break;
        }
        ++i;
    }
    if (i) {
        --i;
    }

    VFSNode *parent = VFSFind(VFSRoot, name);
    if (!parent || !StrCmp(parent->name, parsd[i]) || !parent->create) {
        return;
    }
    PrintF("parent->create(%s)\n", name);
    parent->create(name);
}