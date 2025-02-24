#include <fs/BOTFS.h>
#include <fs/ufs.h>

/*
Bugs:
delete x len file
create >x len file
data overflow
*/

BOTFSHeader BFSGlob;

U32 BOTFSURead(Ptr this, String dir, String name, Ptr buf, U32 size) {
    BOTFSRead(name, buf, 0, size);
    return size;
}
U32 BOTFSUWrite(Ptr this, String dir, String name, Ptr buf, U32 size) {
    BOTFSWrite(name, buf, 0, size);
    return size;
}
U0 BOTFSInit() {
    DATAlloc(0x10);
    DATAlloc(0x11);
    DATAlloc(0x12);
    DATAlloc(0x13);
    DATAlloc(0x14);
    DATAlloc(0x15);
    DATAlloc(0x16);
    DATAlloc(0x17);
    DATAlloc(0x18);
    DATAlloc(0x19);
    DATAlloc(0x1A);
    DATAlloc(0x1B);
    DATRead(0x10, &BFSGlob, 0, sizeof(BOTFSHeader));
    if (BFSGlob.magic != 0x674E1D8F) {
        BOTFSFormat();
    }
    UFSRegister(BOTFSURead, BOTFSUWrite, "home");
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
    MemSet(&BFSGlob, 0, sizeof(BOTFSHeader));
    BFSGlob.magic = 0x674E1D8F;
    BFSGlob.version = 1;
    BFSGlob.blocks_count = 11;
    BFSGlob.files_start = 512;
    BFSGlob.bitmap_start = 2048;
    BFSGlob.data_start = 5;
    BFSGlob.flags = 0;
    BOTFSFlush();
}
U32 BOTFSFind(String name) {
    BOTFSFile *files = BOTFSFGet();

    U32 s = (BFSGlob.bitmap_start - BFSGlob.files_start) / sizeof(BOTFSFile);
    for (U32 i = 0; i < s; ++i) {
        BOTFSFile *file = &files[i];
        if (!StrCmp(file->name, name)) {
            MFree(files);
            return i;
        }
    }
    MFree(files);
    return 0xFFFFFFFF;
}
U0 BOTFSFlush() {
    DATWrite(0x10, &BFSGlob, 0, sizeof(BOTFSHeader));
}

U0 BOTFSRead(String name, Ptr buf, U16 shift, U16 count) {
    BOTFSFile *files = BOTFSFGet();

    U32 s = (BFSGlob.bitmap_start - BFSGlob.files_start) / sizeof(BOTFSFile);
    for (U32 i = 0; i < s; ++i) {
        BOTFSFile *file = &files[i];
        if (!StrCmp(file->name, name)) {
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
        if (!StrCmp(file->name, name)) {
            BOTFSDWrite(file->start+BFSGlob.data_start, file->shift+shift, file->shift+shift+count, buf);
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
        U32 j;
        if (!i) {
            j = 0;
        }
        else {
            j = files[i - 1].start * 512 + files[i - 1].shift + files[i - 1].size;
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
        if (!StrCmp(files[i].name, name)) {
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