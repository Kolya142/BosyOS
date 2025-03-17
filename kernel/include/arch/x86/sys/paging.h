#pragma once
#include <lib/Types.h>

#define PAGE_SIZE 4096
#define PAGE_PRESENT  0x1
#define PAGE_RW       0x2
#define PAGE_USER     0x4

#define PAGE_DIR_ADDR  0x00D00000
#define PAGE_TBL_ADDR  0x00F00000

#define Pages  ((volatile uint32_t*)PAGE_DIR_ADDR)
#define PageT  ((volatile uint32_t*)PAGE_TBL_ADDR)

U0 PagingID(U32 *first, U32 from, U32 size);
U0 PagingInit();

U0 PMap(U32 vaddr, U32 raddr, U32 flags);
U32 PGet(U32 vaddr);
Ptr PAlloc();
U0 PFree(Ptr ptr);
Ptr PallocMap(U32 vaddr, U32 flags);

// in arch/paging.s
extern U0 PagingEnable();

// in arch/paging.s
extern U0 Paginginvlpg(U32 vaddr);