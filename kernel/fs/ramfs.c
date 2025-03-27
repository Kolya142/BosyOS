#include <lib/IO/TTY.h>
#include <fs/ramfs.h>
RFSFile *RFS;

U0 RFSInit() {
    RFS = MAlloc(sizeof(RFSFile) * RFS_SIZE);
    MemSet(RFS, 0, sizeof(RFSFile) * RFS_SIZE);
    VFSDirMk("/tmp", RFSCreateV);
}
U0 RFSAdd(String name, U32 size) {
    for (U32 i = 0; i < RFS_SIZE; ++i) {
        if (!RFS[i].exists) {
            RFS[i].exists = True;
            RFS[i].size = size;
            StrCpy(RFS[i].name, name);
            RFS[i].data = MAlloc(size);
            MemSet(RFS[i].data, 0, size);
            String nname = MAlloc(StrLen(name) + 5);
            StrCpy(nname, "tmp/");
            StrCpy(nname + 4, name);
            nname[4 + StrLen(name)] = 0;
            // PrintF("File: %s\n", nname);
            VFSMount(nname, RFSReadV, RFSWriteV, RFSStatV);
            MFree(nname);
            break;
        }
        else if (!StrCmp(RFS[i].name, name)) {
            break;
        }
    }
}
U0 RFSReSize(String name, U32 size) {
    for (U32 i = 0; i < RFS_SIZE; ++i) {
        if (RFS[i].exists && !StrCmp(RFS[i].name, name)) {
            RFS[i].data = MReAlloc(RFS[i].data, RFS[i].size, size);
            MemSet(RFS[i].data, 0, size);
            RFS[i].size = size;
            break;
        }
    }
}
U32 RFSReadV(String name, Ptr buf, U32 offset, U32 count) {
    // SerialPrintF("ramfs read: %s\n", name);
    while (*name == '/') ++name;
    // SerialPrintF("ramfs read: %s\n", name);
    name += 4;
    // SerialPrintF("ramfs read: %s\n", name);
    for (U32 i = 0; i < RFS_SIZE; ++i) {
        if (RFS[i].exists && !StrCmp(RFS[i].name, name)) {
            count = min(count, RFS[i].size - offset);
            MemCpy(buf, RFS->data + offset, count);
            // PrintF("Reading %d:\n%s\n", count, buf);
            return count;
        }
    }
}
U32 RFSWriteV(String name, Ptr buf, U32 offset, U32 count) {
    // SerialPrintF("ramfs write: %s\n", name);
    while (*name == '/') ++name;
    // SerialPrintF("ramfs write: %s\n", name);
    name += 4;
    // SerialPrintF("ramfs write: %s\n", name);
    for (U32 i = 0; i < RFS_SIZE; ++i) {
        if (RFS[i].exists && !StrCmp(RFS[i].name, name)) {
            if (count > RFS[i].size - offset) {
                RFSReSize(name, count + offset);
            }
            MemCpy(RFS->data + offset, buf, count);
            // PrintF("Writing %d:\n%s\n", count, buf);
            return count;
        }
    }
}
U0 RFSCreateV(String name) {
    while (*name == '/') ++name;
    name += 4;
    RFSAdd(name, 1);
}
U0 RFSStatV(String name, VFSStat *stat) {
    // SerialPrintF("ramfs stat: %s\n", name);
    while (*name == '/') ++name;
    // SerialPrintF("ramfs stat: %s\n", name);
    name += 4;
    // SerialPrintF("ramfs stat: %s\n", name);
    for (U32 i = 0; i < RFS_SIZE; ++i) {
        if (RFS[i].exists && !StrCmp(RFS[i].name, name)) {
            stat->ino = i;
            stat->mode = VFS_IFREG | VFS_UEXEC | VFS_UREAD | VFS_UWRIT;
            stat->size = RFS[i].size;
            stat->time = 0;
            break;
        }
    }
}