// daintree

#include <ast.h>
#include <string.h>
#include <mem.h>

void expr_free(struct expr *expr) {
    switch (expr->type) {
    case EXPR_IDENTIFIER:
        free(expr->identifier);
        break;

    case EXPR_BINARY:
        expr_free(expr->binary.lhs);
        expr_free(expr->binary.rhs);
        break;

    default:
        /* do nothing */
        break;
    }

    free(expr);
}

struct expr *expr_copy(struct expr const *expr) {
    struct expr *copy = malloc(sizeof(*copy));
    
    switch (copy->type = expr->type) {
    case EXPR_NUMBER:
        copy->number = expr->number;
        break;
    case EXPR_IDENTIFIER:
        copy->identifier = strdup(expr->identifier);
        break;
    case EXPR_BINARY:
        copy->binary.type = expr->binary.type;
        copy->binary.lhs = expr_copy(expr->binary.lhs);
        copy->binary.rhs = expr_copy(expr->binary.rhs);
        break;
    }

    return copy;
}

static struct expr *expr_alloc(enum expr_type type) {
    struct expr *expr = malloc(sizeof(*expr));
    expr->type = type;
    return expr;
}

struct expr *expr_number(int number) {
    struct expr *expr = expr_alloc(EXPR_NUMBER);
    expr->number = number;
    return expr;
}

struct expr *expr_identifier(char const *identifier) {
    struct expr *expr = expr_alloc(EXPR_IDENTIFIER);
    expr->identifier = strdup(identifier);
    return expr;
}

struct expr *expr_binary(enum expr_binary_type type, struct expr const *lhs, struct expr const *rhs) {
    struct expr *expr = expr_alloc(EXPR_BINARY);
    expr->binary.type = type;
    expr->binary.lhs = expr_copy(lhs);
    expr->binary.rhs = expr_copy(rhs);
    return expr;
}

static struct stmt *stmt_alloc(enum stmt_type type) {
    struct stmt *stmt = malloc(sizeof(*stmt));
    stmt->type = type;
    return stmt;
}

struct stmt *stmt_assign(char const *identifier, struct expr const *expr) {
    struct stmt *stmt = stmt_alloc(STMT_ASSIGN);
    stmt->assign.identifier = strdup(identifier);
    stmt->assign.value = expr_copy(expr);
    return stmt;
}

struct stmt *stmt_print(struct expr const *expr) {
    struct stmt *stmt = stmt_alloc(STMT_PRINT);
    stmt->print = expr_copy(expr);
    return stmt;
}

void stmt_free(struct stmt *stmt) {
    switch (stmt->type) {
    case STMT_ASSIGN:
        free(stmt->assign.identifier);
        expr_free(stmt->assign.value);
        break;
    case STMT_PRINT:
        expr_free(stmt->print);
        break;
    }

    free(stmt);
}

struct stmt *stmt_copy(struct stmt const *stmt) {
    struct stmt *copy = malloc(sizeof(*copy));
    
    switch (copy->type = stmt->type) {
    case STMT_ASSIGN:
        copy->assign.identifier = strdup(stmt->assign.identifier);
        copy->assign.value = expr_copy(stmt->assign.value);
        break;
    case STMT_PRINT:
        copy->print = expr_copy(stmt->print);
        break;
    }

    return copy;
}

void stmt_list_append(struct stmt_list **list, struct stmt const *stmt) {
    while (*list) {
        list = &(*list)->next;
    }

    *list = malloc(sizeof(**list));
    (*list)->stmt = stmt_copy(stmt);
    (*list)->next = NULL;
}

void stmt_list_free(struct stmt_list *list) {
    while (list) {
        stmt_free(list->stmt);
        struct stmt_list *next = list->next;
        free(list);
        list = next;
    }
}

/* vim: set sw=4 et: */
