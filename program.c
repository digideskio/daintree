// daintree

#include <program.h>
#include <console.h>
#include <mem.h>
#include <string.h>
#include <math.h>

#define VAL_IS_NUMBER(v) ((v).raw & 1)
#define VAL_EXTRACT_NUMBER(v) ((v).raw >> 1)

/*
 * NOTE:
 *
 * irb(main):021:0> (2 ** 30).class
 * => Bignum
 * irb(main):022:0> (2 ** 30 - 1).class
 * => Fixnum
 * irb(main):024:0> (-(2 ** 30)).class
 * => Fixnum
 * irb(main):025:0> (-(2 ** 30)-1).class
 * => Bignum
 */

static object *object_alloc(enum object_type type) {
    object *obj = malloc(sizeof(*obj));
    obj->type = type;
    return obj;
}

object *object_string(char const *str) {
    object *obj = object_alloc(OBJECT_STRING);
    obj->string = strdup(str);
    return obj;
}

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
    case EXPR_UNARY:
        {
            val arg = eval(expr->unary.arg, context);

            if (!VAL_IS_NUMBER(arg)) {
                return val_number(0);
            }

            int an = VAL_EXTRACT_NUMBER(arg);

            switch (expr->unary.type) {
            case EXPR_UNARY_NEG:
                return val_number(-an);
            }
        }
    case EXPR_BINARY:
        {
            val lhs = eval(expr->binary.lhs, context);
            val rhs = eval(expr->binary.rhs, context);

            if (!VAL_IS_NUMBER(lhs) || !VAL_IS_NUMBER(rhs)) {
                return val_number(0);
            }

            int ln = VAL_EXTRACT_NUMBER(lhs),
                rn = VAL_EXTRACT_NUMBER(rhs);

            switch (expr->binary.type) {
            case EXPR_BINARY_PLUS:
                return val_number(ln + rn);
            case EXPR_BINARY_TIMES:
                return val_number(ln * rn);
            case EXPR_BINARY_MINUS:
                return val_number(ln - rn);
            case EXPR_BINARY_DIVIDE:
                return val_number(ln / rn);
            case EXPR_BINARY_MODULO:
                return val_number(ln % rn);
            case EXPR_BINARY_EXP:
                return val_number(powi(ln, rn));
            }
        }
    case EXPR_STRING:
        return (val) object_string(expr->string);
    }
    /* ?? */
    return (val) (uint32_t) 0;
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
                putf("%d\n", VAL_EXTRACT_NUMBER(v));
            } else {
                switch (v.object->type) {
                case OBJECT_STRING:
                    putf("%s\n", v.object->string);
                    break;
                }
            }
            break;
        }
    }
}

Context *context_new(void) {
    Context *context = malloc(sizeof(*context));
    context->env = dict_create(stringdict_crc, NULL);
    return context;
}

void context_free(Context *context) {
    dict_free(context->env);
    free(context);
}

void program_run(Program const *program, Context *context) {
    struct stmt_list const *list = program->stmt_list;

    for (; list; list = list->next) {
        execute(list->stmt, context);
    }
}

/* vim: set sw=4 et: */
