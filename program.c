// daintree

#include <program.h>
#include <string.h>
#include <mem.h>

struct stmt *stmt_new(char const *ident, int num) {
    struct stmt *stmt = malloc(sizeof(*stmt));
    stmt->identifier = strdup(ident);
    stmt->number = num;
    return stmt;
}

/* vim: set sw=4 et: */
