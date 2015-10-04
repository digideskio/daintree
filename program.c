// daintree

#include <program.h>
#include <console.h>
#include <mem.h>
#include <string.h>
#include <math.h>
#include <console.h>
#include <gc.h>

/*
 * NOTE (32-bit):
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

static val eval(struct expr const *expr, Context *context);

static object *object_alloc(enum object_type type) {
    object *obj = malloc(sizeof(*obj));
    obj->type = type;
    return obj;
}

object *object_string(char const *str) {
    object *obj = object_alloc(OBJECT_STRING);
    obj->string = strdup(str);
    return gc_track(obj);
}

object *object_list(struct expr_list const *list, Context *context) {
    object *obj = object_alloc(OBJECT_LIST);
    obj->list = NULL;
    struct val_list **ptr = &obj->list;
    while (list) {
        *ptr = malloc(sizeof(**ptr));
        (*ptr)->value = eval(list->expr, context);
        ptr = &(*ptr)->next;
        list = list->next;
    }
    *ptr = NULL;
    return gc_track(obj);
}

object *object_dict(struct expr_list const *dict, Context *context) {
    object *obj = object_alloc(OBJECT_DICT);
    obj->dict = dict_create(stringdict_crc, (dict_copy_f) strdup, free, NULL);

    while (dict) {
        val key = eval(dict->expr, context);
        dict = dict->next;

        if (!dict) {
            return NULL;
        }

        if (!VAL_IS_OBJECT(key) || VAL_OBJECT(key)->type != OBJECT_STRING) {
            return NULL;
        }

        val value = eval(dict->expr, context);
        dict = dict->next;

        dict_insert(obj->dict, VAL_OBJECT(key)->string, value.object);
    }

    return obj;
}

static int mark_dict(void const *key, void *data, void *extra) {
    val v = (val) (object *) data;
    if (VAL_IS_OBJECT(v)) {
        object_mark(VAL_OBJECT(v));
    }
    return 0;
}

void object_mark(object *object) {
    if (!object) {
        return;
    }

    object->mark = 1;

    switch (object->type) {
    case OBJECT_STRING:
        break;

    case OBJECT_LIST:
        {
            struct val_list *list = object->list;
            while (list) {
                if (VAL_IS_OBJECT(list->value)) {
                    object_mark(VAL_OBJECT(list->value));
                }
                list = list->next;
            }
            break;
        }

    case OBJECT_DICT:
        {
            dict_foreach(object->dict, NULL, mark_dict);
            break;
        }
    }
}

void object_free(object *object) {
    if (!object) {
        return;
    }

    switch (object->type) {
    case OBJECT_STRING:
        free(object->string);
        break;
    case OBJECT_LIST:
        {
            struct val_list *list = object->list;
            while (list) {
                struct val_list *next = list->next;
                free(list);
                list = next;
            }
            break;
        }
    case OBJECT_DICT:
        dict_free(object->dict);
        break;
    }

    free(object);
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

            int an = VAL_NUMBER(arg);

            switch (expr->unary.type) {
            case EXPR_UNARY_NEG:
                return val_number(-an);
            }
        }
    case EXPR_BINARY:
        {
            val lhs = eval(expr->binary.lhs, context);
            val rhs = eval(expr->binary.rhs, context);

            int ln = VAL_NUMBER(lhs),
                rn = VAL_NUMBER(rhs);

            switch (expr->binary.type) {
            case EXPR_BINARY_PLUS:
                if (!VAL_IS_NUMBER(lhs) || !VAL_IS_NUMBER(rhs)) {
                    return (val) (object *)NULL;
                }
                return val_number(ln + rn);
            case EXPR_BINARY_TIMES:
                if (!VAL_IS_NUMBER(lhs) || !VAL_IS_NUMBER(rhs)) {
                    return (val) (object *)NULL;
                }
                return val_number(ln * rn);
            case EXPR_BINARY_MINUS:
                if (!VAL_IS_NUMBER(lhs) || !VAL_IS_NUMBER(rhs)) {
                    return (val) (object *)NULL;
                }
                return val_number(ln - rn);
            case EXPR_BINARY_DIVIDE:
                if (!VAL_IS_NUMBER(lhs) || !VAL_IS_NUMBER(rhs)) {
                    return (val) (object *)NULL;
                }
                return val_number(ln / rn);
            case EXPR_BINARY_MODULO:
                if (!VAL_IS_NUMBER(lhs) || !VAL_IS_NUMBER(rhs)) {
                    return (val) (object *)NULL;
                }
                return val_number(ln % rn);
            case EXPR_BINARY_EXP:
                if (!VAL_IS_NUMBER(lhs) || !VAL_IS_NUMBER(rhs)) {
                    return (val) (object *)NULL;
                }
                return val_number(powi(ln, rn));
            case EXPR_BINARY_ITEM:
                if (!VAL_IS_OBJECT(lhs) || VAL_OBJECT(lhs)->type != OBJECT_DICT) {
                    return (val) (object *)NULL;
                }
                // TODO: temp.
                if (!VAL_IS_OBJECT(rhs) || VAL_OBJECT(rhs)->type != OBJECT_STRING) {
                    return (val) (object *)NULL;
                }
                void *value = dict_search(VAL_OBJECT(lhs)->dict, VAL_OBJECT(rhs)->string);
                return (val) (object *)value;
            }
        }
    case EXPR_STRING:
        return (val) object_string(expr->string);
    case EXPR_LIST:
        return (val) object_list(expr->list, context);
    case EXPR_DICT:
        return (val) object_dict(expr->dict, context);
    case EXPR_ATTR:
        // TODO
        return (val) (object *)NULL;
    }
    /* ?? */
    return (val) (object *)NULL;
}

struct dict_to_str {
    int i;
    struct buffer *buf;
};

static char *val_to_str(val const v);

static int dict_to_str_helper(void const *key, void *data, void *extra) {
    struct dict_to_str *e = extra;

    if (e->i++) {
        append_buffer_str(e->buf, ", ");
    }

    append_buffer_char(e->buf, '"');
    append_buffer_str(e->buf, key);
    append_buffer_str(e->buf, "\": ");
    char *r = val_to_str((val) (object *)data);
    append_buffer_str(e->buf, r);
    free(r);

    return 0;
}

static char *val_to_str(val const v) {
    if (VAL_IS_NUMBER(v)) {
        return sputf("%d", VAL_NUMBER(v));
    }

    if (!v.object) {
        return strdup("None");
    }

    switch (v.object->type) {
    case OBJECT_STRING:
        // TODO: escaping
        return sputf("\"%s\"", v.object->string);

    case OBJECT_LIST:
        {
            struct buffer *buf = alloc_buffer();
            append_buffer_char(buf, '[');
            int i = 0;
            for (struct val_list *list = v.object->list; list; list = list->next) {
                if (i++) {
                    append_buffer_str(buf, ", ");
                }
                char *x = val_to_str(list->value);
                append_buffer_str(buf, x);
                free(x);
            }
            append_buffer_char(buf, ']');
            char *r = strndup(buf->buffer, buf->used);
            free_buffer(buf);
            return r;
        }

    case OBJECT_DICT:
        {
            struct buffer *buf = alloc_buffer();
            append_buffer_char(buf, '{');

            struct dict_to_str e = { 0, buf };
            dict_foreach(v.object->dict, &e, dict_to_str_helper);
            append_buffer_char(buf, '}');
            char *r = strndup(buf->buffer, buf->used);
            free_buffer(buf);
            return r;
        }
    }

    return strdup("?");
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
            char *s = val_to_str(v);
            puts(s);
            putc('\n');
            free(s);
        }
    }
}

Context *context_new(void) {
    Context *context = malloc(sizeof(*context));
    context->env = dict_create(stringdict_crc, (dict_copy_f) strdup, free, NULL);
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
