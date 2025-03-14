#include <fs/romfs.h>

static ROFSSB *ROFS;

static U32 ROFSReadV(String name, Ptr buf, U32 offset, U32 count) {
    ROFSNode *node = (ROFSNode*)(ROFS+sizeof(ROFSSB*));
    for (U32 i = 0; i < ROFS->count; ++i) {
        if (!StrCmp(node->name, name)) {
            MemCpy(buf, (Byte*)node + sizeof(ROFSNode) + offset, count);
            break;
        }
        node += sizeof(ROFSNode) + node->size;
    }
    return 0;
}
static U32 ROFSWriteV(String name, Ptr buf, U32 offset, U32 count) {}
static U0 ROFSStatV(String name, VFSStat *stat) {
    ROFSNode *node = (ROFSNode*)(ROFS+sizeof(ROFSSB*));
    for (U32 i = 0; i < ROFS->count; ++i) {
        if (!StrCmp(node->name, name)) {
            stat->ino = i;
            stat->mode = VFS_REG;
            stat->size = node->size;
            stat->time = 0;
            break;
        }
        node += sizeof(ROFSNode) + node->size;
    }
}

U0 ROFSInit(Byte *buf) {
    ROFS = (ROFSSB*)buf;
    if (StrCmp(ROFS->magic, "-romfs-")) {
        return;
    }
    ROFSNode *node = (ROFSNode*)(buf+sizeof(ROFSSB*));
    for (U32 i = 0; i < ROFS->count; ++i) {
        VFSMount(node->name, ROFSReadV, ROFSWriteV, ROFSStatV);
        node += sizeof(ROFSNode) + node->size;
    }
}