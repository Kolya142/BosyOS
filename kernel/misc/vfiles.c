#include <drivers/misc/random.h>
#include <lib/graphics/Graphics.h>
#include <drivers/input/keyboard.h>
#include <drivers/sys/beep.h>
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
    count = min(count, WIDTH*HEIGHT - offset);
    if (!is_userspace((U32)buf) || !is_userspace((U32)buf+count)) {
        return 0;
    }
    MemCpy(VRM+offset, buf, count);
    return count;
}
static U32 ScreenRead(String, Ptr buf, U32 offset, U32 count) {
    count = min(count, WIDTH*HEIGHT - offset);
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
    count = min(count, 0x100000 - offset);
    if (offset > 0x100000 || offset + count > 0x100000) {
        return 0;
    }
    MemCpy(buf, (Ptr)offset, count);
    return count;
}
static U32 MemWrite(String, Ptr buf, U32 offset, U32 count) {
    count = min(count, 0x100000 - offset);
    if (offset > 0x100000 || offset + count > 0x100000) {
        return 0;
    }
    MemCpy((Ptr)offset, buf, count);
    return count;
}

static U32 SPCWrite(String, Ptr buf, U32 offset, U32 count) {
    BeepSPC(((U16*)buf)[0], ((U16*)buf)[1]);
    return 4;
}

static U32 COLWrite(String, Ptr buf, U32 offset, U32 count) {
    if (count != 4*16) {
        return 0;
    }
    MemCpy(VRMColors, buf, 4*16);
    return 4*16;
}

static U32 KBDRead(String, Ptr buf, U32 offset, U32 count) {
    if (count != 256) {
        return 0;
    }
    MemCpy(buf, KBState.keys, 256);
    return 256;
}

U0 VFilesInit() {
    VFSDirMk("/dev", Null);
    VFSMount("/dev/urandom", URandom, NullF, Null);
    VFSMount("/dev/screen", ScreenRead, ScreenWrite, Null);
    VFSMount("/dev/null", NullF, NullF, Null);
    VFSMount("/dev/zeros", Zeros, NullF, Null);
    VFSMount("/dev/mem", MemRead, MemWrite, Null);
    VFSMount("/dev/spc", NullF, SPCWrite, Null);
    VFSMount("/dev/color", NullF, COLWrite, Null);
    VFSMount("/dev/keyboard", KBDRead, NullF, Null);
}
