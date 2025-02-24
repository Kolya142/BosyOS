#include <misc/vfiles.h>
#include <drivers/pit.h>
#include <lib/Random.h>
#include <lib/MemLib.h>

U32 VFilesUpTimeRead(Ptr this, Ptr buf, U32 size) {
    if (size < 4) {
        return 0;
    }
    *(U32*)buf = PITTime;
    return 4;
}
U32 VFilesZeroesRead(Ptr this, Ptr buf, U32 size) {
    MemSet(buf, 0, size);
    return size;
}
U32 VFilesRandomRead(Ptr this, Ptr buf, U32 size) {
    for (U32 i = 0; i < size; ++i) {
        ((U8*)buf)[i] = RandomU() % 0xff;
    }
    return size;
}
U32 VFilesZWrite(Ptr this, Ptr buf, U32 size) {
    return 0;
}

U0 VFilesInit() {
    VFSRegister(1, "home", "uptime", VFilesUpTimeRead, VFilesZWrite);
    VFSRegister(2, "dev", "zeroes", VFilesZeroesRead, VFilesZWrite);
    VFSRegister(3, "dev", "random", VFilesRandomRead, VFilesZWrite);
}