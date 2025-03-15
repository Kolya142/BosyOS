#include <arch/x86/sys/paging.h>
#include <lib/memory/MemLib.h>
#include <lib/time/Time.h>
#include <lib/IO/TTY.h>

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
    Sleep(1000);
    for (U32 i = 0; i < 1024*1024; ++i) {
        PageT[i] = (i * PAGE_SIZE) | PAGE_RW | PAGE_PRESENT | PAGE_USER;
    }
    for (U32 i = 0; i < 1024; ++i) {
       Pages[i] = ((U32)&PageT[i * 1024]) | PAGE_RW | PAGE_PRESENT | PAGE_USER;
    }
    PagingEnable();
}

U0 PMap(U32 vaddr, U32 raddr, U32 flags) {
    U32 pdid = vaddr >> 22;
    U32 ptid = (vaddr >> 12) & 0x3FF;
    U32* pt = (U32*)(Pages[pdid] & ~0xFFF);
    pt[ptid] = raddr | flags | PAGE_PRESENT | PAGE_USER;
    Paginginvlpg(vaddr);
}
U32 PGet(U32 vaddr) {
    U32 pdid = vaddr >> 22;
    U32 ptid = (vaddr >> 12) & 0x3FF;
    U32* pt = (U32*)(Pages[pdid] & ~0xFFF);
    return pt[ptid] & ~0xFF;
}
Ptr PAlloc() {
    for (U32 i = 0; i < 1024 * 1024 / 8; i++) {
        if (PageBitmap[i] != 0xFF) {
            for (U8 j = 0; j < 8; j++) {
                if (!(PageBitmap[i] & (1 << j))) {
                    PageBitmap[i] |= (1 << j);
                    return 0x1000000 + (Ptr)((i * 8 + j) * PAGE_SIZE);
                }
            }
        }
    }
    return Null;
}
Ptr PFree(Ptr ptr) {
    U32 index = ((U32)ptr - 0x1000000) / PAGE_SIZE;
    PageBitmap[index / 8] &= ~(1 << (index % 8));
}
Ptr PallocMap(U32 vaddr, U32 flags) {
    Ptr paddr = PAlloc();
    if (paddr) {
        PMap(vaddr, (U32)paddr, flags);
    }
    return paddr;
}