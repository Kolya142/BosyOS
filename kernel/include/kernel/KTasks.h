#pragma once
#include <lib/Types.h>
#include <arch/x86/sys/idt.h>

struct Task;
typedef struct Task Task;

#define TASK_WORKING 1
#define TASK_CREATED 2

#define SIGNALS 32

typedef struct Task {
    U32 id;
    U16 flags;
    INTRegs3 regs;
    Task *next;
    U32 esp;
    U32 ttyid;
    U8 fpu[512];

    U32 saved_eip;
    U32 saved_esp;

    U0 (*signals_handles[SIGNALS])(U16 signal);

    U8 signals[SIGNALS / 8];
} __attribute__((packed)) Task;
extern Bool TaskingIs;
extern Task *TaskHead;
extern Task *TaskTail;
extern Task *TaskLast;

U0 TaskInit();
U0 TaskNext();
U0 TaskClose();
U32 TaskNew(U32 eip, U16 ds, U16 cs);
U0 TaskKill(U32 id);

U32 TFork();

// U0 TSleep(U32 millis);