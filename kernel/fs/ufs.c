#include <lib/MemLib.h>
#include <lib/String.h>
#include <fs/ufs.h>

UFSNode *ufs;

U32 UFSRead(String dir, String name, Ptr buf, U32 size) {
    for (U32 i = 0; i < 128; ++i) {
        if (ufs[i].read && !StrCmp(ufs[i].root, dir)) {
            return ufs[i].read(&ufs[i], dir, name, buf, size);
        }
    }
}
U32 UFSWrite(String dir, String name, Ptr buf, U32 size) {
    for (U32 i = 0; i < 128; ++i) {
        if (ufs[i].write && !StrCmp(ufs[i].root, dir)) {
            return ufs[i].write(&ufs[i], dir, name, buf, size);
        }
    }
}
U0 UFSRegister(Ptr read, Ptr write, String root) {
    for (U32 i = 0; i < 128; ++i) {
        if (!StrCmp(ufs[i].root, root)) {
            return;
        }
        if (!ufs[i].read) {
            ufs[i].read = read;
            ufs[i].write = write;
            StrCpy(ufs[i].root, root);
            return;
        }
    }
}
U0 UFSInit() {
    MAlloc(sizeof(UFSNode)*128);
    MemSet(ufs, 0, sizeof(UFSNode)*128);
}