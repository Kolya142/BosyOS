#include <kernel/KDogWatch.h>
#include <kernel/KTasks.h>
#include <drivers/time/pit.h>
#include <lib/memory/MemLib.h>
#include <arch/x86/cpu/cpu.h>
#include <lib/IO/TTY.h>

// fixing bugs at 1741179362s from unix stamp
// fixed 90% bugs at 1741183725s from unix stamp
// proccesses start at 1741763233 from unix stamp
// problem "i need ring3" found at 1742038534 from unix stamp

static U32 task_count = 0;
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
    TaskTail->flags &= ~TASK_CREATED;
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
    task_count = 0;
    TaskHead = Null;
    TaskTail = Null;
    TaskLast = Null;
}
U0 TaskNext() {
    if (!TaskTail || !TaskHead) return;
    // for (;;) {
    TaskTail = (TaskTail->next) ? TaskTail->next : TaskHead;
    //     break;
    // }
}

U32 TaskNew(U32 eip, U16 ds, U16 cs) { // recommended to check if an error is detected
    asmv("cli");
    U32 *stack = MAlloc(8192);
    if (!(U32)stack) {
        asmv("sti");
        return 0xFFFFFFFF;
    }
    
    U32 esp = (U32)(&stack[2047]) & ~0xF;
    KDogWatchLogF("Process with esp %X and eip %x created\n", esp, eip);

    Task *task = MCAlloc(sizeof(Task), 1);
    if (!task) {
        asmv("sti");
        return 0xFFFFFFFF;
    }

    task->regs.eflagsp = 0x202; // initial cpu flags
    task->regs.eflags  = 0x202; // initial cpu flags
    
    task->regs.esp = esp; // initial stack
    task->regs.ebp = esp;

    task->regs.eip = eip; // InstructionPointer
    
    task->regs.ds = ds; // data segments
    task->regs.es = ds;
    task->regs.fs = ds;
    task->regs.gs = ds;
    task->regs.ss = ds;

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
            if (task == TaskTail) {
                TaskNext();
            }
            
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

U32 TFork() {
    Task *parent = TaskTail;

    if (!(parent->flags & TASK_WORKING)) {
        return 0;
    }

    U32 childi = TaskNew(parent->regs.eip, parent->regs.ds, parent->regs.cs);

    if (childi == 0xFFFFFFFF) {
        return 0xFFFFFFFF;
    }

    Task *child = TaskHead;
    Task *prev = Null;

    while (child) {
        if (child->id == childi) {
            break;
        }
        child = child->next;
    }
    MemCpy(&child->regs, &parent->regs, sizeof(INTRegs));
    MemCpy((Ptr)child->regs.useresp, (Ptr)parent->regs.useresp, 4096);
    child->regs.ebp = child->regs.useresp;
    child->regs.eax = 0;
    return child->id;
}