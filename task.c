// daintree

#include <task.h>
#include <mem.h>
#include <string.h>
#include <console.h>

struct task_list *current_task;
struct task_list *tasks = NULL;

struct task_list *add_task(struct task *task) {
    struct task_list *tl_entry = malloc(sizeof(*tl_entry));
    tl_entry->task = task;
    tl_entry->next = tasks;
    tasks = tl_entry;
    return tl_entry;
}

struct task *create_task(char const *name, uint32_t entry) {
    struct task *task = malloc(sizeof(*task));
    task->name = strdup(name);

    if (!entry) {
        task->stack = NULL;
    } else {
        void *stack = malloc(0x2000);
        task->stack = stack + 0x2000 - sizeof(*task->stack);
        task->stack->ds = 0x10;
        task->stack->edi = task->stack->esi =
            task->stack->ebp = task->stack->esp =
            task->stack->ebx = task->stack->edx =
            task->stack->ecx = task->stack->eax =
            0;
        task->stack->eip = entry;
        task->stack->cs = 0x08;
        task->stack->eflags = 0x200;
    }

    task->waiting_irq = 0;

    return task;
}

void tasks_init(void) {
    current_task = add_task(create_task("idle", 0));
}

struct callback_registers *tasks_switch(struct callback_registers *stack) {
    current_task->task->stack = stack;
    while (1) {
        current_task = 
            current_task->next ? current_task->next : tasks;

        struct task *task = current_task->task;
        if (task->waiting_irq) {
            if (task->waiting_irq_hits > 0) {
                --task->waiting_irq_hits;
                task->waiting_irq = 0;
                return task->stack;
            }
        } else {
            return task->stack;
        }
    }
}

// vim: set sw=4 et:
