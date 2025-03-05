#pragma once
#include <lib/Types.h>
#include <arch/idt.h>

struct Task;
typedef struct Task Task;

#define TASK_WAITING 1
#define TASK_WORKING 2
#define TASK_CREATED 4

typedef struct Task {
    U32 id;
    U16 flags;
    INTRegs regs;
    Task *next;
    U32 esp;
    U32 time;
} Task;
extern Bool TaskingIs;
extern Task *TaskHead;
extern Task *TaskTail;
extern Task *TaskLast;

U0 TaskInit();
U0 TaskNext();
U0 TaskClose();
extern U0 TaskYeild();
U32 TaskNew(U32 eip, U16 ds, U16 cs);
U0 TaskKill(U32 id);
U0 TSleep(U32 millis);