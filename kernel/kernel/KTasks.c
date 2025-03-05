#include <kernel/KTasks.h>
#include <lib/MemLib.h>

Bool TaskingIs = False;
Task *TaskHead = Null;
Task *TaskTail = Null;
Task *TaskLast = Null;
U8 *TaskStacks;

U0 TaskClose() {
    TaskKill(TaskTail->id);
    asmv("movl %0, %%esp; movl %1, %%ebp; movl $1, %%esi; jmp *%2"
        :: "r"(TaskTail->regs.useresp),
           "r"(TaskTail->regs.ebp),
           "r"(TaskTail->regs.eip));
}

U0 TaskInit() {
    TaskStacks = MCAlloc(4096*500, 1);
}
U0 TaskNext() {
    if (!TaskTail || !TaskHead) return;
    TaskTail = (TaskTail->next) ? TaskTail->next : TaskHead;
}
U0 TaskYeild() {
    Bool code = False;
    asmv("movl %%esp, %0; movl %%ebp, %1; mov $0, %%esi; movl 4(%%ebp), %2"
        : "=r"(TaskTail->regs.useresp),
          "=r"(TaskTail->regs.ebp),
          "=r"(TaskTail->regs.eip),
          "=S"(code)
        );
    if (code) return;
    TaskNext();
    asmv("movl %0, %%esp; movl %1, %%ebp; movl $1, %%esi; jmp *%2"
        :: "r"(TaskTail->regs.useresp),
           "r"(TaskTail->regs.ebp),
           "r"(TaskTail->regs.eip));
}

U32 TaskNew(U32 eip) {
    asmv("cli");
    static U32 task_count = 0;
    if (task_count >= 500) return -1; 

    U32 *stack = (U32*)(&TaskStacks[task_count * 4096]);
    stack[1023] = 0;
    stack[1022] = eip;
    stack[1021] = 0x08;
    stack[1020] = 0x202;
    
    U32 esp = (U32)&stack[1020];
    

    Task *task = MCAlloc(sizeof(Task), 1);
    if (task == Null) return -1;

    task->regs.eip = eip;
    task->regs.ebp = 0;
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
                if (TaskHead == Null) TaskTail = Null;
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
    TaskNext();
}
