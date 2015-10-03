// daintree

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <arch.h>
#include <ast.h>
#include <dict.h>

struct object_struct;

typedef union {
   uint32_t raw;
   struct object_struct *object;
} val;

struct val_list {
    val value;
    struct val_list *next;
};

/* temp. This will be GC'd and based on the actual class system. */
typedef struct object_struct {
    enum object_type {
        OBJECT_STRING,
        OBJECT_LIST,
    } type;
    union {
        char *string;
        struct val_list *list;
    };
} object;

typedef struct {
    struct dict *env;
} Context;

Context *context_new(void);
void context_free(Context *context);
void program_run(Program const *program, Context *context);

#endif

/* vim: set sw=4 et: */
