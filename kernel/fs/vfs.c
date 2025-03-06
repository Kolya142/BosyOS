#include <fs/vfs.h>

VFSNode VFSRoot;

U0 VFSInit() {
    VFSRoot = (VFSNode) {
        .children = ListInit(sizeof(VFSNode)),
        .list = Null,
        .name = Null,
        .parent = Null,
        .read = Null,
        .write = Null
    };
}
VFSNode* VFSFind(VFSNode parent, String name) {
    for (U32 i = 0; i < parent.children.count; ++i) {
        if (!StrCmp(((VFSNode*)parent.children.arr)[i].name, name)) {
            return &((VFSNode*)parent.children.arr)[i];
        }
    }
    return Null;
}
List VFSParsePath(const String path) {
    Char buf[16];
    List lpath = ListInit(sizeof(String));
    U32 j = 0, i = 0, s = StrLen(path);
    for (i = 0; i < s; ++i) {
        if (path[i] == '/') {
            buf[j++] = 0;
            U32 len = StrLen(buf);
            String pathc = MAlloc(j+1);
            StrCpy(pathc, buf);
            ListAppend(&lpath, &pathc);
            pathc[j] = 0;
            j = 0;
        }
        else {
            if(j < sizeof(buf) - 1) {
                buf[j++] = path[i];
            }
        }
    }
    if(j > 0) {
        buf[j++] = 0;
        String pathc = MAlloc(j+1);
        StrCpy(pathc, buf);
        ListAppend(&lpath, &pathc);
        pathc[j] = 0;
    }
    return lpath;
}
U0 VFSFreePath(List *lpath) {
    for (U32 i = 0; i < lpath->count; ++i) {
        String comp = ((String*)lpath->arr)[i];
        MFree(comp);
    }
    ListDestroy(lpath);
}
U32 VFSRead(String name, Ptr buf, U32 count) {
    List lpath = VFSParsePath(name);
    U32 res = 0xFFFFFFFF;
    VFSNode *curr = &VFSRoot;
    for (U32 i = 0; i < lpath.count - 1; ++i) {
        curr = VFSFind(*curr, ((String*)lpath.arr)[i]);
        if (!curr) {
            break;
        }
    }
    if (curr) {
        curr->read(((String*)lpath.arr)[lpath.count - 1], buf, count);
    }
    VFSFreePath(&lpath);
    return res;
}
U32 VFSWrite(String name, Ptr buf, U32 count) {
    List lpath = VFSParsePath(name);
    U32 res = 0xFFFFFFFF;
    VFSNode *curr = &VFSRoot;
    for (U32 i = 0; i < lpath.count - 1; ++i) {
        curr = VFSFind(*curr, ((String*)lpath.arr)[i]);
        if (!curr) {
            break;
        }
    }
    if (curr) {
        curr->write(((String*)lpath.arr)[lpath.count - 1], buf, count);
    }
    VFSFreePath(&lpath);
    return res;
}
U0 VFSList(String name, VFSNode *buf, U32 count) {
    
}
U0 VFSMount(String path, U0(*read)(String, Ptr, U32), U0(*write)(String, Ptr, U32), U0(*list)(struct VFSNode*, U32)) {
    List lpath = VFSParsePath(path);
    String last = ((String*)lpath.arr)[lpath.count - 1];
    String llast = MAlloc(StrLen(last));
    StrCpy(llast, last);
    VFSNode node = (VFSNode) {
        .children = ListInit(sizeof(VFSNode)),
        .list = list,
        .name = llast,
        .parent = Null,
        .read = read,
        .write = write
    };
    VFSNode *curr = &VFSRoot;
    for (U32 i = 0; i < lpath.count - 1; ++i) {
        curr = VFSFind(*curr, ((String*)lpath.arr)[i]);
    }
    ListAppend(&curr->children, &node);
    VFSFreePath(&lpath);
}