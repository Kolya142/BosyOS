#include <kernel/KTasks.h>
#include <lib/MemLib.h>

Bool TaskingIs = False;
Task *TaskHead = Null;
Task *TaskTail = Null;
Task *TaskLast = Null;
U8 *TaskStacks;

U0 TaskInit() {
    TaskStacks = MCAlloc(4096*500, 1);
}
U0 TaskNext() {
    if (!TaskTail || !TaskHead) return;
    if (!TaskTail->next) {
        TaskTail = TaskHead;
    }
    else {
        TaskTail = TaskTail->next;
    }
}

U32 TaskNew(U32 eip) {
    asmv("cli");
    static U32 task_count = 0;
    if (task_count >= 500) return -1; 

    U32 esp = (U32)(&TaskStacks[task_count * 4096 + 4092]);

    Task *task = MCAlloc(sizeof(Task), 1);
    if (task == Null) return -1;

    task->regs.eip = eip;
    task->regs.useresp = esp;
    task->id = (TaskLast) ? TaskLast->id + 1 : 0;
    task->next = Null;

    if (TaskHead == Null) {
        TaskHead = task;
        TaskTail = task;
        TaskLast = task;
    }
    else {
        TaskLast->next = task;
        TaskLast = task;
    }
    ++task_count;
    asmv("sti");
    return TaskLast->id;
}
// Underrated function
U0 TaskKill(U32 id) {
    Task *task = TaskHead;
    Task *prev = Null;

    while (task) {
        if (task->id == id) {
            if (prev) {
                prev->next = task->next;
            }
            else {
                TaskHead = task->next;
            }
            if (task == TaskLast) {
                TaskLast = prev;
            }
            if (task == TaskTail) {
                TaskTail = prev ? prev : TaskHead;
            }
            MFree(task);
            break;
        }
        prev = task;
        task = task->next;
    }
}
