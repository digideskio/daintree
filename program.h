// daintree

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <arch.h>
#include <ast.h>
#include <dict.h>

/* temp. This will be GC'd and based on the actual class system. */
typedef struct {
    enum object_type {
        OBJECT_STRING
    } type;
    union {
        char *string;
    };
} object;

typedef union {
   uint32_t raw;
   object *object;
} val;

typedef struct {
    struct dict *env;
} Context;

Context *context_new(void);
void context_free(Context *context);
void program_run(Program const *program, Context *context);

#endif

/* vim: set sw=4 et: */
