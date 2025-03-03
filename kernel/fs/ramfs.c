#include <fs/ramfs.h>
RFSFile *RFS;

U0 RFSInit() {
    RFS = MAlloc(sizeof(RFSFile) * RFS_SIZE);
    MemSet(RFS, 0, sizeof(RFSFile) * RFS_SIZE);
}
U0 RFSAdd(String name, U32 size) {
    for (U32 i = 0; i < RFS_SIZE; ++i) {
        if (!RFS[i].exists) {
            RFS[i].exists = True;
            RFS[i].size = size;
            StrCpy(RFS[i].name, name);
            RFS[i].data = MAlloc(size);
            break;
        }
    }
}
U0 RFSReSize(String name, U32 size) {
    for (U32 i = 0; i < RFS_SIZE; ++i) {
        if (RFS[i].exists && !StrCmp(RFS[i].name, name)) {
            RFS[i].data = MReAlloc(RFS[i].data, RFS[i].size, size);
            RFS[i].size = size;
            break;
        }
    }
}
U32 RFSOpen(String name) {
    for (U32 i = 0; i < RFS_SIZE; ++i) {
        if (RFS[i].exists && !StrCmp(RFS[i].name, name)) {
            RFSFileDescriptor *r = MAlloc(sizeof(RFSFileDescriptor));
            r->head = 0;
            r->pos = i;
            return (U32)r;
        }
    }
    return Null;
}
U32 RFSWrite(U32 fd, Ptr buf, U32 count) {
    RFSFileDescriptor *r = (RFSFileDescriptor*)fd;
    U32 c = (r->head+count)%RFS[r->pos].size-r->head;
    MemCpy(RFS[r->pos].data+r->head, buf, c);
    r->head += c;
    return c;
}
U32 RFSRead(U32 fd, Ptr buf, U32 count) {
    RFSFileDescriptor *r = (RFSFileDescriptor*)fd;
    U32 c = (r->head+count)%RFS[r->pos].size-r->head;
    MemCpy(buf, RFS[r->pos].data+r->head, c);
    r->head += c;
    return c;
}
U0 RFSClose(U32 fd) {
    RFSFileDescriptor *r = (RFSFileDescriptor*)fd;
    MFree(r);
}