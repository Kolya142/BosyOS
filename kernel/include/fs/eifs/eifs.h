#include <fs/eifs/direntry.h>
#include <fs/eifs/defines.h>
#include <fs/eifs/inode.h>

typedef struct EIFSuperBlock {
    U32 magic; // EIFS

    U32 iblock_alloc_start;
    U32 iblock_alloc_end;
    U32 iblock_start;
    U32 iblock_end;

    U32 block_alloc_start;
    U32 block_alloc_end;
    U32 block_start;
    U32 block_end;
} __attribute__((packed)) EIFSuperBlock;

U32 EIFIAlloc();
U32 EIFBAlloc();

EIFINode EIFIGet(U32 i);
U0 EIFBGet(U32 i, U8 *buf);

U0 EIFInit();