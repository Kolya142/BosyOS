#pragma once
#include <drivers/sys/pci.h>
#include <lib/Types.h>

typedef struct BlkDev {
    U32 id;
    U32 sector_size;
    U0(*read)(struct BlkDev *blkdev, U32 start, Ptr buf, U32 size);
    U0(*write)(struct BlkDev *blkdev, U32 start, Ptr buf, U32 size);
    Bool exists;
    U32 controller_data[12];
} BlkDev;

extern BlkDev Blks[16];

U0 BlkDevInit();