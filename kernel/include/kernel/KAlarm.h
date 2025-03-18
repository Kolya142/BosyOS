#pragma once
#include <lib/memory/Collects.h>
#include <lib/Types.h>

typedef struct Alarm {
    struct Alarm *next;
    U0(*event)(Ptr args);
    Ptr args;
    U32 time;
} Alarm;

extern Alarm *AlarmHead;
extern Alarm *AlarmTail;

U0 AlarmCreate(U32 delay, U0(*event)(Ptr args), Ptr args);
Alarm *AlarmGet();
U0 AlarmRemove(Alarm *a);