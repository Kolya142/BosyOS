#pragma once
#include <lib/Types.h>
#include <arch/idt.h>

struct Task;
typedef struct Task Task;

typedef struct Task {
    U32 id;
    INTRegs regs;
    Task *next;
} Task;
extern Bool TaskingIs;
extern Task *TaskHead;
extern Task *TaskTail;
extern Task *TaskLast;

U0 TaskInit();
U0 TaskNext();
U32 TaskNew(U32 eip);
U0 TaskKill(U32 id);