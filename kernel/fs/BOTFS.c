#include <fs/BOTFS.h>

/*
Bugs:
*/

BOTFSHeader BFSGlob;

U0 BOTFSInit() {
    for (U16 i = 0x10; i <= 0x1B; ++i) {
        DATAlloc(i);
    }    
    DATRead(0x10, &BFSGlob, 0, sizeof(BOTFSHeader));
    if (BFSGlob.magic != 0x674E1D8F) {
        BOTFSFormat();
    }
}
U0 BOTFSDRead(U16 seg, U16 start, U16 end, Ptr buf) {
    U32 i = 0;
    U32 count = end - start;
    while (i < count) {
        U8 segment[512];

        U16 os = count - i;
        U16 mmax = 512 - ((start + i) % 512);
        os = min(os, mmax);

        DATRead(0x10 + seg + (start + i) / 512, // id
                segment, // buf
                (start + i) % 512, // start
                os); // count
        MemCpy(buf + i, segment, os);

        i += os;
    }
}
U0 BOTFSDWrite(U16 seg, U16 start, U16 end, Ptr buf) {
    U32 i = 0;
    U32 count = end - start;
    while (i < count) {
        U8 segment[512];
        DATRead(0x10 + seg + (start + i) / 512, // id
                segment, // buf
                0, // start
                512); // count

        U16 os = count - i;
        U16 mmax = 512 - ((start + i) % 512);
        os = min(os, mmax);

        MemCpy(segment + (start + i) % 512, buf + i, os);
        DATWrite(0x10 + seg + (start + i) / 512, // id
                segment, // buf
                0, // start
                512); // count

        i += os;
    }
}
U0 BOTFSFormat() {
    U8 segment[512] = {0};
    DATWrite(0x10, &BFSGlob, 0, sizeof(BOTFSHeader));
    
    MemSet(&BFSGlob, 0, sizeof(BOTFSHeader));
    BFSGlob.magic = 0x674E1D8F;
    BFSGlob.version = 1;
    BFSGlob.blocks_count = 11;
    BFSGlob.files_start = 512;
    BFSGlob.bitmap_start = 2048;
    BFSGlob.data_start = 5;
    BFSGlob.flags = 0;
    BOTFSFlush();
    MemSet(segment, 0, 512);
    for (U32 i = 0x11; i <= 0x1B; ++i)
        DATWrite(i, segment, 0, 512);
}
U32 BOTFSFind(String name) {
    BOTFSFile *files = BOTFSFGet();

    U32 s = (BFSGlob.bitmap_start - BFSGlob.files_start) / sizeof(BOTFSFile);
    for (U32 i = 0; i < s; ++i) {
        BOTFSFile *file = &files[i];
        if (file->exists && !StrCmp(file->name, name)) {
            MFree(files);
            return i;
        }
    }
    MFree(files);
    return 0xFFFFFFFF;
}
BOTFSFileStat BOTFSStat(String name) {
    BOTFSFile *files = BOTFSFGet();

    U32 s = (BFSGlob.bitmap_start - BFSGlob.files_start) / sizeof(BOTFSFile);
    for (U32 i = 0; i < s; ++i) {
        BOTFSFile *file = &files[i];
        if (file->exists && !StrCmp(file->name, name)) {
            BOTFSFileStat statt;
            statt.flags = file->flags;
            statt.start = (U32)file->start * 512 + (U32)file->shift;
            statt.size = file->size;
            statt.exists = True;
            StrCpy(statt.name, file->name);
            MFree(files);
            return statt;
        }
    }
    MFree(files);
    return (BOTFSFileStat) {.exists = False};
}
U0 BOTFSFlush() {
    DATWrite(0x10, &BFSGlob, 0, sizeof(BOTFSHeader));
}

U0 BOTFSRead(String name, Ptr buf, U16 shift, U16 count) {
    BOTFSFile *files = BOTFSFGet();

    U32 s = (BFSGlob.bitmap_start - BFSGlob.files_start) / sizeof(BOTFSFile);
    for (U32 i = 0; i < s; ++i) {
        BOTFSFile *file = &files[i];
        if (file->exists && !StrCmp(file->name, name)) {
            BOTFSDRead(file->start+BFSGlob.data_start, file->shift+shift, file->shift+shift+count, buf);
            MFree(files);
            return;
        }
    }
    MFree(files);
}

U0 BOTFSWrite(String name, Ptr buf, U16 shift, U16 count) {
    BOTFSFile *files = BOTFSFGet();

    U32 s = (BFSGlob.bitmap_start - BFSGlob.files_start) / sizeof(BOTFSFile);
    for (U32 i = 0; i < s; ++i) {
        BOTFSFile *file = &files[i];
        if (file->exists && !StrCmp(file->name, name)) {
            BOTFSDWrite(file->start+BFSGlob.data_start, file->shift+shift, file->shift+shift+count, buf);
            MFree(files);
            return;
        }
    }
    MFree(files);
}

U0 BOTFSCreate(String name, U16 size) {
    if (BOTFSFind(name) != 0xFFFFFFFF) {
        return;
    }
    U8 *bitmap = BOTFSBGet();

    BOTFSFile *files = BOTFSFGet();

    U32 s = (BFSGlob.bitmap_start - BFSGlob.files_start) / sizeof(BOTFSFile);
    for (U32 i = 0; i < s; ++i) {
        if (bitmap[i/8] & (1 << (i%8)))
            continue;
        if (i + 1 < s && files[i+1].exists && (files[i+1].start + files[i+1].shift) < size + (files[i-1].start + files[i-1].shift + files[i-1].size))
            continue;
        U32 j;
        if (!i) {
            j = BFSGlob.data_start + 0;
        }
        else {
            j = BFSGlob.data_start + files[i - 1].start * 512 + files[i - 1].shift + files[i - 1].size;
        }
        files[i].start = j / 512;
        files[i].shift = j % 512;
        files[i].size = size;
        StrCpy(files[i].name, name);
        files[i].flags = 0b111;
        files[i].exists = True;
        bitmap[i / 8] |= 1 << (i%8);
        break;
    }
    
    BOTFSBPut(bitmap);
    BOTFSFPut(files);

    MFree(bitmap);
    MFree(files);
}
U0 BOTFSDelete(String name) {
    U8 *bitmap = BOTFSBGet();
    BOTFSFile *files = BOTFSFGet();

    U32 s = (BFSGlob.bitmap_start - BFSGlob.files_start) / sizeof(BOTFSFile);
    for (U32 i = 0; i < s; ++i) {
        if (files[i].exists && !StrCmp(files[i].name, name)) {
            bitmap[i / 8] &= ~(1 << (i % 8));
            MemSet(&files[i], 0, sizeof(BOTFSFile));
            break;
        }
    }

    BOTFSBPut(bitmap);
    BOTFSFPut(files);

    MFree(bitmap);
    MFree(files);
}

U0 BOTFSList(String *buf, U16 max_files) {
    BOTFSFile *files = BOTFSFGet();
    U32 s = (BFSGlob.bitmap_start - BFSGlob.files_start) / sizeof(BOTFSFile);

    U32 j = 0;
    for (U32 i = 0; i < s && j < max_files; ++i) {
        if (files[i].exists) {
            buf[j] = files[i].name;
            ++j;
        }
    }
    MFree(files);
}

BOTFSFile *BOTFSFGet() {
    BOTFSFile *files = MAlloc(BFSGlob.bitmap_start - BFSGlob.files_start);
    BOTFSDRead(BFSGlob.files_start / 512, BFSGlob.files_start % 512, BFSGlob.bitmap_start - BFSGlob.files_start, files);
    return files;
}
U8 *BOTFSBGet() {
    U8 *bitmap = MAlloc(BFSGlob.data_start * 512 - BFSGlob.bitmap_start);
    BOTFSDRead(BFSGlob.bitmap_start / 512, BFSGlob.bitmap_start % 512, BFSGlob.data_start * 512 - BFSGlob.bitmap_start, bitmap);
    return bitmap;
}
U0 BOTFSFPut(BOTFSFile *f) {
    BOTFSDWrite(BFSGlob.files_start / 512, BFSGlob.files_start % 512, BFSGlob.bitmap_start - BFSGlob.files_start, f);
}
U0 BOTFSBPut(U8 *f) {
    BOTFSDWrite(BFSGlob.bitmap_start / 512, BFSGlob.bitmap_start % 512, BFSGlob.data_start * 512 - BFSGlob.bitmap_start, f);
}