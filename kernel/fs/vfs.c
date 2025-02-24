#include <lib/KeyboardLib.h>
#include <lib/MemLib.h>
#include <lib/String.h>
#include <lib/TTY.h>
#include <fs/vfs.h>
#include <fs/ufs.h>

VFSNode *vfs;

U32 VFSTestRead(Ptr this, Ptr buf, U32 size) {
    KBRead(buf, size);
    return StrLen(buf);
}
U32 VFSTestWrite(Ptr this, Ptr buf, U32 size) {
    for (U32 i = 0; i < size; ++i) {
        TTYUPrintC(((Char*)buf)[i]);
    }
    return size;
}

U0 VFSInit() {
    vfs = MAlloc(sizeof(VFSNode) * 32);
    MemSet(vfs, 0, sizeof(VFSNode) * 32);
    String dir = "dev";
    String nam = "tty";
    VFSRegister(0, dir, nam, VFSTestRead, VFSTestWrite);
    TTYUPrint("VFS[0] Name: ");
    TTYUPrint(vfs[0].name);
    TTYUPrint("\n");
}
U32 VFSURead(Ptr this, String dir, String name, Ptr buf, U32 size) {
    return VFSRead(dir, name, buf, size);
}
U32 VFSUWrite(Ptr this, String dir, String name, Ptr buf, U32 size) {
    return VFSWrite(dir, name, buf, size);
}
U0 VFSRegister(U32 id, String dir, String name, Ptr read, Ptr write) {
    if (id >= 32) return;
    if (vfs[id].exists) return;
    for (U32 i = 0; i < 32; ++i) {
        if (vfs[i].exists && !StrCmp(vfs[i].dir, dir) && !StrCmp(vfs[i].name, name)) {
            return;
        }
    }
    MemSet(vfs[id].dir, 0, sizeof(vfs[id].dir));
    MemSet(vfs[id].name, 0, sizeof(vfs[id].name));

    StrCpy(vfs[id].dir, dir);
    StrCpy(vfs[id].name, name);
    UFSRegister(VFSURead, VFSUWrite, vfs[id].dir);

    vfs[id].read = read;
    vfs[id].write = write;
    vfs[id].exists = True;
}
U32 VFSRead(String dir, String name, Ptr buf, U32 size) {
    for (U32 i = 0; i < 32; ++i) {
        if (vfs[i].exists && !StrCmp(vfs[i].dir, dir) && !StrCmp(vfs[i].name, name)) {
            return vfs[i].read(&vfs[i], buf, size);
        }
    }
    return 0xFFFFFFFF;
}
U32 VFSWrite(String dir, String name, Ptr buf, U32 size) {
    for (U32 i = 0; i < 32; ++i) {
        if (vfs[i].exists && !StrCmp(vfs[i].dir, dir) && !StrCmp(vfs[i].name, name)) {
            return vfs[i].write(&vfs[i], buf, size);
        }
    }
    return 0xFFFFFFFF;
}
String *VFSList(String dir) {
    U32 j = 0;
    String *names = MAlloc(sizeof(String) * 32);
    for (U32 i = 0; i < 32; ++i) {
        if (vfs[i].exists && !StrCmp(vfs[i].dir, dir)) {
            names[j] = vfs[i].name;
            ++j;
        }
    }
    return names;
}