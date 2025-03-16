#include <drivers/misc/random.h>
#include <fs/vfs.h>

static U32 URandom(String, Ptr buf, U32 offset, U32 count) {
    for (U32 i = 0; i < count; ++i) {
        ((U8*)buf)[i] = RandomU();
    }
    return count;
}

U0 VFilesInit() {
    VFSMount("urandom", URandom, URandom, Null);
}