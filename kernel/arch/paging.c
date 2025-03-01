#include <arch/paging.h>
#include <lib/MemLib.h>
#include <lib/Time.h>
#include <lib/TTY.h>

U8 PageBitmap[1024 * 1024 / 8] = {0};

U0 PagingID(U32 *first, U32 from, U32 size) {
    from &= ~0xFF;
    for (; size; from += PAGE_SIZE, size -= PAGE_SIZE, ++first) {
        *first = from | PAGE_PRESENT | PAGE_RW;
    }
}

U0 PagingInit() {
    PrintF("Page: %p\n", Pages);
    MemSet(Pages, 0, PAGE_SIZE);
    MemSet(PageT, 0, 1024 * PAGE_SIZE);
    SleepM(1000);
    for (U32 i = 0; i < 1024; ++i) {
        Pages[i] = (PAGE_TBL_ADDR + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW;
    }
    {
        U32 *pt = (U32*)(PAGE_TBL_ADDR + 0 * PAGE_SIZE);
        uint32_t ofs = (0x300000 >> 12) & 0x3FF;
        PagingID(pt+ofs, 0x300000, 0x400000 - 0x300000);
    }
    {
        U32 *pt = (U32*)(PAGE_TBL_ADDR + 1 * PAGE_SIZE);
        uint32_t ofs = (0x400000 >> 12) & 0x3FF;
        PagingID(pt+ofs, 0x400000, 0x500000 - 0x400000);
    }
    PagingEnable();
}

U0 PMap(U32 vaddr, U32 raddr, U32 flags) {
    U32 pdid = vaddr >> 22;
    U32 ptid = (vaddr >> 12) & 0x3FF;
    U32* pt = (U32*)(Pages[pdid] & ~0xFFF);
    pt[ptid] = raddr | flags | PAGE_PRESENT;
    Paginginvlpg(vaddr);
}
Ptr PAlloc() {
    for (U32 i = 0; i < 1024 * 1024 / 8; i++) {
        if (PageBitmap[i] != 0xFF) {
            for (U8 j = 0; j < 8; j++) {
                if (!(PageBitmap[i] & (1 << j))) {
                    PageBitmap[i] |= (1 << j);
                    return (Ptr)((i * 8 + j) * PAGE_SIZE);
                }
            }
        }
    }
    return Null;
}