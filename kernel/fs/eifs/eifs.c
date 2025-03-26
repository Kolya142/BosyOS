#include <fs/eifs/eifs.h>
#include <fs/iso9660.h>
static U32 start;
static U32 size;

U32 EIFIAlloc() {

}
U32 EIFBAlloc() {

}

EIFINode EIFIGet(U32 i) {

}
U0 EIFBGet(U32 i, U8 *buf) {

}

U0 EIFInit() {
    ISO9660DirEntry *de = ISO9660Get("drive.");
    start = de->extent_lba_le * 4;
    size = (de->data_length_le + 511) / 512;
}