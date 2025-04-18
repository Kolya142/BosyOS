#include <lib/strings/String.h>
#include <kernel/KPanic.h>
#include <drivers/disk/ide.h>
#include <lib/IO/TTY.h>
#include <fs/iso9660.h>

static U32 root_dir_lba;
static U32 root_dir_size;
static U8 dir[2048];

U0 ISO9660Init() {
    U8 iso[2048];
    if (!ATARead(0, iso, 16 * 4, 4)) {KPanic("Failed to load ISO9660\n", False);}
    root_dir_lba = *(U32*)(iso + 156 + 2);
    root_dir_size = *(U32*)(iso + 156 + 10);
    if (!ATARead(0, dir, root_dir_lba * 4, 4)) {KPanic("Failed to load ISO9660\n", False);}
}
ISO9660DirEntry *ISO9660Get(String name) {
    U32 s = StrLen(name);
    for (U32 off = 0; off < 2048;) {
        ISO9660DirEntry *entry = (ISO9660DirEntry*)&dir[off];
        if (!entry->length) {
            break;
        }
        SerialPrintF("File: %s\n", entry->name);
        if (entry->name_len == s + 2) {
            Bool cmp = True;
            for (U32 i = 0; i < s; ++i) {
                if (entry->name[i] == ';') {
                    break;
                }
                if ((entry->name[i] >= 'A' && entry->name[i] <= 'Z' ? entry->name[i] - 'A' + 'a' : entry->name[i]) != name[i]) {
                    cmp = False;
                    break;
                }
            }
            if (cmp) {
                return entry;
            }
        }
        off += entry->length;
    }
    return NULL;
}