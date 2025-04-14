#include <drivers/time/pit.h>
#include <kernel/KAlarm.h>

Alarm *AlarmHead = Null;
Alarm *AlarmTail = Null;

U0 AlarmCreate(U32 delay, U0(*event)(Ptr args), Ptr args) {
    if (!event) return;

    Alarm *a = MCAlloc(sizeof(Alarm), 1);
    a->args = args;
    a->time = PITTime + delay;
    a->event = event;

    if (!AlarmHead) {
        a->next = Null;
        AlarmHead = a;
        AlarmTail = a;
    }
    else {
        a->next = AlarmTail->next;
        AlarmTail->next = a;
    }
}
Alarm *AlarmGet() {
    Alarm *a = AlarmTail;
    if (a) {
        AlarmTail = AlarmTail->next;
    }
    return a;
}
U0 AlarmRemove(Alarm *a) {
    if (a == AlarmHead) {
        AlarmHead = Null;
        AlarmTail = Null;
        return;
    }
    Alarm *b = AlarmHead;
    for (;;) {
        if (b->next == a) {
            break;
        }
        b = b->next;
    }
    b->next = a->next;
    if (a == AlarmTail) {
        AlarmGet();
    }
    MFree(a);
}