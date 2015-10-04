// daintree

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <arch.h>
#include <ast.h>
#include <dict.h>

#define VAL_IS_NUMBER(v) ((v).raw & 1)
#define VAL_IS_OBJECT(v) (!VAL_IS_NUMBER(v))
#define VAL_NUMBER(v) ((v).raw >> 1)
#define VAL_OBJECT(v) ((v).object)

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
    uint8_t mark;
    union {
        char *string;
        struct val_list *list;
    };
} object;

void object_mark(object *object);
void object_free(object *object);

typedef struct {
    struct dict *env;
} Context;

Context *context_new(void);
void context_free(Context *context);
void program_run(Program const *program, Context *context);

#endif

/* vim: set sw=4 et: */
