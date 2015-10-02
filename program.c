// daintree

#include <program.h>
#include <console.h>
#include <mem.h>

#define VAL_IS_NUMBER(v) ((v).raw & 1)

static val context_get(Context *context, char const *key) {
    return (val) (uint32_t) dict_search(context->env, key);
}

static void context_put(Context *context, char const *key, val value) {
    dict_insert(context->env, key, value.object);
}

static val val_number(int number) {
    return (val) ((uint32_t) (number << 1) | 1);
}

static val eval(struct expr const *expr, Context *context) {
    switch (expr->type) {
    case EXPR_IDENTIFIER:
        return context_get(context, expr->identifier);
    case EXPR_NUMBER:
        return val_number(expr->number);
    default:
        return (val) (uint32_t) 0;
    }
}

static void execute(struct stmt const *stmt, Context *context) {
    switch (stmt->type) {
    case STMT_ASSIGN:
        context_put(
            context, stmt->assign.identifier, eval(stmt->assign.value, context));
        break;

    case STMT_PRINT:
        {
            val v = eval(stmt->print, context);
            if (VAL_IS_NUMBER(v)) {
                putf("%d\n", v.raw >> 1);
            } else {
                putf("??\n");
            }
            break;
        }
    }
}

Context *context_new(void) {
    Context *context = malloc(sizeof(*context));
    context->env = dict_create(stringdict_crc, free);
    return context;
}

void program_run(Program const *program, Context *context) {
    struct stmt_list const *list = program->stmt_list;

    for (; list; list = list->next) {
        execute(list->stmt, context);
    }
}

/* vim: set sw=4 et: */
