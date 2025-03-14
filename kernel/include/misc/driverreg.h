#pragma once
#include <lib/Types.h>
#include <fs/vfs.h>
typedef struct Driver {
    U32 d1;
    U32 d2;
    Ptr func;
    String name;
} Driver;

extern Driver Drivers[50];

U0 DriverReg(U32 d1, U32 d2, Ptr func, String name);
U0 DriverCall(U32 d1, U32 d2, U32 id, U32 *value);