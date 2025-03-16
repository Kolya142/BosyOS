#include <lib/IO/TTY.h>
#include <fs/romfs.h>

ROFSSB *ROFS;

static U32 ROFSReadV(String name, Ptr buf, U32 offset, U32 count) {
    ROFSNode *node = (ROFSNode*)((U8*)ROFS+sizeof(ROFSSB));
    for (U32 i = 0; i < ROFS->count; ++i) {
        PrintF("File name: %s, File size: %d\n", node->name, node->size);
        if (!StrCmp(node->name, name)) {
            if (offset >= node->size) return 0;
            count = min(count, node->size - offset);
            MemCpy(buf, (Byte*)node + sizeof(ROFSNode) + offset, count);
            return count;
        }
        node += sizeof(ROFSNode) + node->size;
    }
    return 0;
}
static U32 ROFSWriteV(String name, Ptr buf, U32 offset, U32 count) {
    return 0;
}
static U0 ROFSStatV(String name, VFSStat *stat) {
    ROFSNode *node = (ROFSNode*)((U8*)ROFS+sizeof(ROFSSB));
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
    PrintF("loading romfs\n");
    ROFS = (ROFSSB*)buf;
    PrintF("magic: %s", ROFS->magic);
    if (!StrStartsWith(ROFS->magic, "-romfs-")) {
        return;
    }
    PrintF("loading romfs\n");
    ROFSNode *node = (ROFSNode*)(buf+sizeof(ROFSSB));
    for (U32 i = 0; i < ROFS->count; ++i) {
        PrintF("File: %s\n", node->name);
        VFSMount(node->name, ROFSReadV, ROFSWriteV, ROFSStatV);
        node += sizeof(ROFSNode) + node->size;
    }
}