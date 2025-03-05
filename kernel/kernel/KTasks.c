#include <kernel/KDogWatch.h>
#include <kernel/KTasks.h>
#include <drivers/pit.h>
#include <lib/MemLib.h>
#include <arch/cpu.h>
#include <lib/TTY.h>

// fixing bugs from 1741179362s from unix stamp
// fixed 90% bugs from 1741183725s from unix stamp

static U32 task_count = 0;
Bool TaskingIs = False;
Task *TaskHead = Null;
Task *TaskTail = Null;
Task *TaskLast = Null;

U0 TaskClose() {
    asmv("cli");
    U32 id = TaskTail->id;
    TaskNext();
    TaskKill(id);
    TaskTail->flags &= ~TASK_WORKING;
    asmv("sti");
    if (TaskHead) {
        for(;;);
    }
    else {
        KDogWatchLog("No more tasks. Halting", False);
        CpuHalt();
    }
    // for(;;);
    // asmv("movl %0, %%esp; movl %1, %%ebp; movl $1, %%esi; jmp *%2"
    //     :: "r"(TaskTail->regs.useresp),
    //        "r"(TaskTail->regs.ebp),
    //        "r"(TaskTail->regs.eip));
}
U0 TSleep(U32 millis) { // TODO: fix waiting
    // asmv("cli");
    // TaskTail->flags |= TASK_WAITING;
    // TaskTail->time = PITTime + millis;
    // asmv("sti");
}
U0 TaskInit() {
}
U0 TaskNext() {
    if (!TaskTail || !TaskHead) return;
    for (;;) {
        TaskTail = (TaskTail->next) ? TaskTail->next : TaskHead;
        if (TaskTail->flags & TASK_WAITING) {
            if (PITTime >= TaskTail->time) {
                TaskTail->flags &= ~TASK_WAITING;
            }
            else {
                continue;
            }
        }
        break;
    }
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

U32 TaskNew(U32 eip, U16 ds, U16 cs) {
    asmv("cli");
    if (task_count >= 500) return -1; 

    U32 *stack = MAlloc(4096);
    stack[4096] = 0;
    stack[4095] = eip;
    stack[4094] = 0x08;
    stack[4093] = 0x202;
    
    U32 esp = (U32)&stack[4092];
    PrintF("Process with esp $!B%4x$!F created\n", esp);

    Task *task = MCAlloc(sizeof(Task), 1);
    if (task == Null) {
        asmv("sti");
        return -1;
    }

    task->regs.eip = eip;
    task->regs.ebp = esp;
    task->regs.useresp = esp;
    task->regs.eflagsp = 0x202;
    task->regs.eflags = 0x202;
    task->regs.ds = ds;
    task->regs.es = ds;
    task->regs.fs = ds;
    task->regs.gs = ds;
    task->regs.cs = cs;
    task->flags = TASK_CREATED;
    
    task->esp = esp;
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
            MFree((Ptr)task->esp);
            MFree(task);
            break;
        }
        prev = task;
        task = task->next;
    }
    TaskNext();
}
