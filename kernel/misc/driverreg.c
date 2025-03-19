#include <misc/driverreg.h>

Driver Drivers[50];

static U32 DriverVRead(String name, Ptr buf, U32 offset, U32 count) {
    for (U32 i = 0; i < 50; ++i) {
        if (!StrCmp(Drivers[i].name, name)) {
            U0(*func)(U32, U32*);
            func = (U0(*)(U32, U32*))Drivers[i].func;
            func(((U32*)buf)[0], &((U32*)buf)[1]);
            return count;
        }
    }
    return count;
}
static U32 DriverVWrite(String name, Ptr buf, U32 offset, U32 count) {
    return DriverVRead(name, buf, 0, count);
}

static U0 DriverVStat(String name, VFSStat *stat) {
    for (U32 i = 0; i < 50; ++i) {
        if (!StrCmp(Drivers[i].name, name)) {
            stat->ino = i;
            stat->size = 0;
            stat->mode = VFS_IFDEV | VFS_UREAD | VFS_UWRIT;
            stat->time = 0;
        }
    }
}

U0 DriverReg(U32 d1, U32 d2, Ptr func, String name) {
    for (U32 i = 0; i < 50; ++i) {
        if (!Drivers[i].d1) {
            Drivers[i].d1 = d1;
            Drivers[i].d2 = d2;
            Drivers[i].name = name;
            Drivers[i].func = func;
            U32 s = StrLen(name);
            String nname = MAlloc(s+5);
            MemCpy(nname, "dev/", 4);
            MemCpy(nname + 4, name, s);
            nname[s] = 0;
            VFSMount(nname, DriverVRead, DriverVWrite, DriverVStat);
            MFree(nname);
            break;
        }
    }
}
U0 DriverCall(U32 d1, U32 d2, U32 id, U32 *value) {
    for (U32 i = 0; i < 50; ++i) {
        if (Drivers[i].d1 == d1 && Drivers[i].d2 == d2) {
            U0(*func)(U32, U32*);
            func = (U0(*)(U32, U32*))Drivers[i].func;
            func(id, value);
            break;
        }
    }
}