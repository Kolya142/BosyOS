#pragma once
#include <lib/Types.h>

typedef struct ISO9660DirEntry {
    U8 length;
    U8 ext_attr_length;
    U32 extent_lba_le;
    U32 extent_lba_be;
    U32 data_length_le;
    U32 data_length_be;
    U8 date[7];
    U8 flags;
    U8 file_unit_size;
    U8 interleave_gap;
    U16 volume_seq_le;
    U16 volume_seq_be;
    U8 name_len;
    Char name[];
} __attribute__((packed)) ISO9660DirEntry;


U0 ISO9660Init();
ISO9660DirEntry *ISO9660Get(String name);