#pragma once
#include <drivers/disk/ide.h>
#include <lib/memory/MemLib.h>
#include <lib/Types.h>
#include <fs/vfs.h>

#define EIF_REG   0x8000
#define EIF_RONLY 0x4000
#define EIF_DEV   0x2000

#define EIF_READ  0x0800
#define EIF_WRITE 0x0400
#define EIF_EXEC  0x0200

#define EIF_BLOCK 512

// FrEe
#define EIF_ERRFE 0xFFFFFFFF
// FounD
#define EIF_ERRFD 0xFFFFFFFE
// PerMission denied
#define EIF_ERRPM 0xFFFFFFFD

typedef struct EIFINode {
    U16 mode;
    U16 blocks[8];
    Char name[10];
    U32 time;
} __attribute__((packed)) EIFINode;
typedef struct EIFSuperBlock {
    U32 shift;
    U8 creator_os;
    U16 magic; // 0xABE1
} __attribute__((packed)) EIFSuperBlock;

#if defined(__clang__) || defined(__GNUC__)
#define EIFFirstFree(b) ((b == 0xFF) ? 0xFFFFFFFF : (__builtin_ffs(~b) - 1))
#else
static inline U32 EIFFirstFree(U8 byte) {
    if (byte == 0xFF) return 0xFFFFFFFF;
    for (U32 i = 0; i < 8; ++i) {
        if (!(byte & (1 << i)))
            return i;
    }
    return 0xFFFFFFFF;
}
#endif

U32 EIFIAlloc();
U32 EIFBAlloc();
U0 EIFISet(U32 id, EIFINode *node);
U0 EIFIGet(U32 id, EIFINode *node);
static inline U0 EIFBSet(U32 id, Ptr buf);
static inline U0 EIFBGet(U32 id, Ptr buf);
U0 EIFCreate(const String name, U16 mode);
U32 EIFFind(const String name);
U32 EIFWrite(const String name, Ptr buf, U32 offset, U32 count);
U32 EIFReadDir(U0 (*callback) (U32 inode_i, EIFINode *inode));
U32 EIFRead(const String name, Ptr buf, U32 offset, U32 count);

U32 EIFReadV(String name, Ptr buf, U32 count);
U32 EIFWriteV(String name, Ptr buf, U32 count);
U32 EIFReadDirV(String, U0(*reader)(String, VFSStat*));

U0 EIFInit();