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
    PrintF("exiting task\n");
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
U0 TaskInit() {
}
U0 TaskNext() {
    if (!TaskTail || !TaskHead) return;
    for (;;) {
        TaskTail = (TaskTail->next) ? TaskTail->next : TaskHead;
        break;
    }
}

U32 TaskNew(U32 eip, U16 ds, U16 cs) { // recommended to check if an error is detected
    asmv("cli");
    U32 *stack = MAlloc(4096 * sizeof(U32));
    if (!(U32)stack) {
        asmv("sti");
        return 0xFFFFFFFF;
    }
    
    U32 esp = (U32)(&stack[4094]);
    PrintF("Process with esp $!B%4x$!F created\n", esp);

    Task *task = MCAlloc(sizeof(Task), 1);
    if (!task) {
        asmv("sti");
        return 0xFFFFFFFF;
    }

    task->regs.eflagsp = 0x202; // initial cpu flags
    task->regs.eflags  = 0x202; // initial cpu flags
    
    task->regs.useresp = esp; // initial stack
    task->regs.ebp = esp;

    task->regs.eip = eip; // InstructionPointer
    
    task->regs.ds = ds; // data segments
    task->regs.es = ds;
    task->regs.fs = ds;
    task->regs.gs = ds;

    task->regs.cs = cs; // code segment
    
    task->flags = TASK_CREATED; // initial flags
    
    task->esp = esp; // initial esp

    task->id = (TaskLast) ? TaskLast->id + 1 : 0; // set tid
    task->next = Null;

    if (!TaskHead) { // if first task
        TaskHead = task;
        TaskTail = task;
        TaskLast = task;
    }
    else {
        TaskLast->next = task;
        TaskLast = task;
    }
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
