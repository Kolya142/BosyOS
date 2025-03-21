#include <drivers/misc/random.h>
#include <lib/graphics/Graphics.h>
#include <lib/IO/TTY.h>
#include <fs/vfs.h>

static inline Bool is_userspace(U32 addr) {
    return 1 || addr >= 0x0C000000;
}

static U32 URandom(String, Ptr buf, U32 offset, U32 count) {
    if (!is_userspace((U32)buf) || !is_userspace((U32)buf+count)) {
        return 0;
    }
    for (U32 i = 0; i < count; ++i) {
        ((U8*)buf)[i] = RandomU();
    }
    return count;
}
static U32 ScreenWrite(String, Ptr buf, U32 offset, U32 count) {
    count = min(count, 320*200 - offset);
    if (!is_userspace((U32)buf) || !is_userspace((U32)buf+count)) {
        return 0;
    }
    MemCpy(VRM+offset, buf, count);
    return count;
}
static U32 ScreenRead(String, Ptr buf, U32 offset, U32 count) {
    count = min(count, 320*200 - offset);
    MemCpy(buf, VRM+offset, count);
    return count;
}

static U32 NullF(String, Ptr buf, U32 offset, U32 count) {
    return 0;
}

static U32 Zeros(String, Ptr buf, U32 offset, U32 count) {
    MemSet(buf, 0, count);
    return count;
}

static U32 MemRead(String, Ptr buf, U32 offset, U32 count) {
    MemCpy(buf, (Ptr)offset, count);
    return count;
}
static U32 MemWrite(String, Ptr buf, U32 offset, U32 count) {
    MemCpy((Ptr)offset, buf, count);
    return count;
}


U0 VFilesInit() {
    VFSDirMk("/dev");
    VFSMount("/dev/urandom", URandom, NullF, Null);
    VFSMount("/dev/screen", ScreenRead, ScreenWrite, Null);
    VFSMount("/dev/null", NullF, NullF, Null);
    VFSMount("/dev/zeros", Zeros, NullF, Null);
    VFSMount("/dev/mem", MemRead, MemWrite, Null);
}