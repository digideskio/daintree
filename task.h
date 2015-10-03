// daintree

#ifndef __TASK_H__
#define __TASK_H__

#include <interrupts.h>

struct task {
    char *name;
    struct callback_registers *stack;
};

struct task_list {
    struct task *task;
    struct task_list *next;
};

struct task_list *add_task(struct task *task);
struct task *create_task(char const *name, uint32_t entry);
void tasks_init(void);
struct callback_registers *tasks_switch(struct callback_registers *stack);

#endif

// vim: set sw=4 et:
