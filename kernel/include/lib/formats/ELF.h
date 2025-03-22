#pragma once
#include <lib/Types.h>

typedef struct ELFHeader {
    U32 magic; // 0x464C457F
    
    U8 arch;
    U8 endian;
    U8 version;
    U8 osabi;
    U8 abiver;
    U8 pad[7];

    U16 type;
    U16 machine;

    U32 version2;
    U32 entry;
    U32 phoff;
    U32 shoff;
    U32 flags;
    
    U16 ehsize;
    U16 phentsize;
    U16 phnum;
    U16 shentsize;
    U16 shnum;
    U16 shstrndx;
} __attribute__((packed)) ELFHeader;

typedef struct ELFProgramHeader {
    U32 type;
    U32 offset;
    U32 vaddr;
    U32 paddr;
    U32 filesz;
    U32 memsz;
    U32 flags;
    U32 align;
} __attribute__((packed)) ELFProgramHeader;

U32 ELFLoad(U8 *buf);