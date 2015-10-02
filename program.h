// daintree

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <arch.h>
#include <ast.h>
#include <dict.h>

typedef union {
   uint32_t raw;
   void *object;
} val;

typedef struct {
    struct dict *env;
} Context;

Context *context_new(void);
void program_run(Program const *program, Context *context);

#endif

/* vim: set sw=4 et: */
