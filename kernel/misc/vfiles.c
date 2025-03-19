#include <drivers/misc/random.h>
#include <lib/graphics/Graphics.h>
#include <lib/IO/TTY.h>
#include <fs/vfs.h>

static U32 URandom(String, Ptr buf, U32 offset, U32 count) {
    for (U32 i = 0; i < count; ++i) {
        ((U8*)buf)[i] = RandomU();
    }
    return count;
}
static U32 ScreenWrite(String, Ptr buf, U32 offset, U32 count) {
    struct patch {
        U16 x;
        U16 y;
        U16 w;
        U8 data[];
    } __attribute__((packed));
    if (count < sizeof(struct patch)) return 0;
    struct patch *patch = buf;
    if (!patch->w) {
        return 0;
    }
    for (U32 i = 0; i < count - 2 - 2 - 2; ++i) {
        // PrintF("%d, %d -> %1x \n", patch->x + i % patch->w, patch->y + i / patch->w, patch->data[i]);
        VRMPSet(patch->x + i % patch->w, patch->y + i / patch->w, patch->data[i]);
    }
    return count;
}
static U32 ScreenRead(String, Ptr buf, U32 offset, U32 count) {
    return 0;
}


U0 VFilesInit() {
    VFSDirMk("/dev");
    VFSMount("/dev/urandom", URandom, URandom, Null);
    VFSMount("/dev/screen", ScreenRead, ScreenWrite, Null);
}