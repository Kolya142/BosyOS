#include <drivers/pit.h>
#include <lib/String.h>
#include <lib/TTY.h>
#include <fs/eifs.h>
EIFSuperBlock EFSB;

static U32 SS;

U32 EIFIAlloc() {
    U8 buf[8 * EIF_BLOCK];
    ATARead(buf, SS + 0, 8);

    for (U32 byte = 0; byte < 8 * EIF_BLOCK; ++byte) {
        if (buf[byte] != 0xFF) {
            U32 bit = EIFFirstFree(buf[byte]);
            buf[byte] |= (1 << bit);
            ATAWrite(buf, SS + 0, 8);
            return byte * 8 + bit;
        }
    }
    return EIF_ERRFE;
}

U32 EIFBAlloc() {
    U8 buf[8 * EIF_BLOCK];
    ATARead(buf, SS + 8, 8);

    for (U32 byte = 0; byte < 8 * EIF_BLOCK; ++byte) {
        if (buf[byte] != 0xFF) {
            U32 bit = EIFFirstFree(buf[byte]);
            buf[byte] |= (1 << bit);
            ATAWrite(buf, SS + 8, 8);
            return byte * 8 + bit;
        }
    }
    return EIF_ERRFE;
}

U0 EIFISet(U32 id, EIFINode *node) {
    U8 buf[EIF_BLOCK];
    id *= sizeof(EIFINode);
    U32 sec = id / EIF_BLOCK;
    U32 shft = id % EIF_BLOCK;
    ATARead(buf, SS + 16 + sec, 1);
    MemCpy(buf + shft, node, sizeof(EIFINode));
    ATAWrite(buf, SS + 16 + sec, 1);
}
U0 EIFIGet(U32 id, EIFINode *node) {
    U8 buf[EIF_BLOCK];
    id *= sizeof(EIFINode);
    U32 sec = id / EIF_BLOCK;
    U32 shft = id % EIF_BLOCK;
    ATARead(buf, SS + 16 + sec, 1);
    MemCpy(node, buf + shft, sizeof(EIFINode));
}
static inline U0 EIFBSet(U32 id, Ptr buf) {
    ATAWrite(buf, SS + 32 + id, 1);
}
static inline U0 EIFBGet(U32 id, Ptr buf) {
    ATARead(buf, SS + 32 + id, 1);
}

U0 EIFCreate(const String name, U16 mode) {
    if (EIFFind(name) != EIF_ERRFD) {
        return;
    }
    U32 inode_num = EIFIAlloc();
    if (inode_num == EIF_ERRFE) {
        return;
    }

    EIFINode inode;
    MemSet(&inode, 0, sizeof(EIFINode));
    
    inode.time = BosyTime + BOSY_EPOCH;
    
    inode.mode = mode;

    U32 s = StrLen(name);
    MemCpy(inode.name, name, min(10, s));

    EIFISet(inode_num, &inode);
}

U32 EIFFind(const String name) {
    for (U32 i = 0; i < 16 * EIF_BLOCK / sizeof(EIFINode); ++i) {
        EIFINode node;
        EIFIGet(i, &node);
        if (!node.mode) {
            continue;
        }
        if (!StrCmp(node.name, name)) {
            return i;
        }
    }
    return EIF_ERRFD;
}
U32 EIFWrite(const String name, Ptr buf, U32 offset, U32 count) {
    U32 inode_num = EIFFind(name);
    if (inode_num == EIF_ERRFD) {
        return EIF_ERRFD;
    }

    EIFINode inode;
    EIFIGet(inode_num, &inode);
    inode.time = BosyTime - BOSY_EPOCH;
    if (!(inode.mode & EIF_WRITE)) {
        return EIF_ERRPM;
    }

    U32 end = offset + count;
    U32 start = offset;

    U8 buffer[8 * EIF_BLOCK];
    MemSet(buffer, 0, sizeof(buffer));

    for (U32 i = 0; i < 8; ++i) {
        if (inode.blocks[i]) {
            EIFBGet(inode.blocks[i], buffer + i * EIF_BLOCK);
        }
    }
    U32 needed = (end + EIF_BLOCK - 1) / EIF_BLOCK;
    for (U32 i = 0; i < 8; ++i) {
        if (!inode.blocks[i] && (i < needed)) {
            U32 new_block = EIFBAlloc();
            if (new_block == EIF_ERRFE) {
                return EIF_ERRFE;
            }
            inode.blocks[i] = new_block;
        }
    }    
    EIFISet(inode_num, &inode);

    MemCpy(buffer + start, buf, count);

    for (U32 i = 0; i < 8; ++i) {
        if (inode.blocks[i]) {
            EIFBSet(inode.blocks[i], buffer + i * EIF_BLOCK);
        }
    }
    return count;
}
U32 EIFReadDir(U0 (*callback) (U32 inode_i, EIFINode *inode)) {
    U32 c = 0;
    for (U32 i = 0; i < 16 * EIF_BLOCK / sizeof(EIFINode); ++i) {
        EIFINode node;
        EIFIGet(i, &node);
        if (node.mode) {
            callback(i, &node);
            ++c;
        }
    }
    return c;
}
U32 EIFRead(const String name, Ptr buf, U32 offset, U32 count) {
    U32 inode_num = EIFFind(name);
    if (inode_num == EIF_ERRFD) {
        return EIF_ERRFD;
    }

    EIFINode inode;
    EIFIGet(inode_num, &inode);
    inode.time = BosyTime - BOSY_EPOCH;
    if (!(inode.mode & EIF_READ)) {
        return EIF_ERRPM;
    }

    U32 end = offset + count;
    U32 start = offset;

    U8 buffer[8 * EIF_BLOCK];
    MemSet(buffer, 0, sizeof(buffer));

    for (U32 i = 0; i < 8; ++i) {
        if (inode.blocks[i]) {
            EIFBGet(inode.blocks[i], buffer + i * EIF_BLOCK);
        }
    }

    MemCpy(buf, buffer + start, count);
    
    return count;
}
U0 EIFInit() {
    U8 buf[512];
    ATARead(buf, 7, 1);
    MemCpy(&EFSB, buf, sizeof(EIFSuperBlock));

    if (EFSB.magic != 0x4945) {
        return;
    }

    SS = 7 + EFSB.shift;

    PrintF("RAW:\n");

    for (int i = 0; i < 16; i++) {
        PrintF("%1X ", buf[i]);
    }
    PrintF("\n");

    PrintF("EIFS Magic: %2x\n", EFSB.magic);
    PrintF("EIFS Creator OS: %s\n", (EFSB.creator_os == 0) ? "Linux" : ((EFSB.creator_os == 1) ? "Unix-like" : "Unknown"));

    // EIFCreate("test.mez", EIF_REG | EIF_READ | EIF_WRITE);
    // // PrintF("Writing: %x\n", EIFWrite("test.mez", "Hello, world@", 0, 14));
    // U8 buff[14];
    // EIFRead("test.mez", buff, 0, 14);
    // PrintF("file data: %s\n", buff);
}
U32 EIFReadV(String name, Ptr buf, U32 count) {
    VFSRead(name, buf, count);
}
U32 EIFWriteV(String name, Ptr buf, U32 count) {
    VFSWrite(name, buf, count);
}
U32 EIFReadDirV(String, U0(*reader)(String, VFSStat*)) {
    U32 c = 0;
    for (U32 i = 0; i < 16 * EIF_BLOCK / sizeof(EIFINode); ++i) {
        EIFINode node;
        EIFIGet(i, &node);
        if (node.mode) {
            VFSStat s;
            s.ino = i;
            s.mode = VFS_REG | VFS_EXEC | VFS_READ | VFS_WRIT;
            s.time = node.time;
            U32 C = 0;
            for (U32 i = 0; i < 8; ++i) {
                if (node.blocks[i]) {
                    ++C;
                }
            }
            s.size = C;
            reader(node.name, &s);
            ++c;
        }
    }
    return c;
}