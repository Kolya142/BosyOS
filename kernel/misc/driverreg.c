#include <misc/driverreg.h>

typedef struct Driver {
    U32 d1;
    U32 d2;
    Ptr func;
} Driver;

static Driver drivearr[50];

U0 DriverReg(U32 d1, U32 d2, Ptr func) {
    for (U32 i = 0; i < 50; ++i) {
        if (!drivearr[i].d1) {
            drivearr[i].d1 = d1;
            drivearr[i].d2 = d2;
            drivearr[i].func = func;
            break;
        }
    }
}
U0 DriverCall(U32 d1, U32 d2, U32 id, U32 *value) {
    for (U32 i = 0; i < 50; ++i) {
        if (drivearr[i].d1 == d1 && drivearr[i].d2 == d2) {
            U0(*func)(U32, U32*);
            func = (U0(*)(U32, U32*))drivearr[i].func;
            func(id, value);
            break;
        }
    }
}