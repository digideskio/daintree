// daintree

#ifndef __AST_H__
#define __AST_H__

struct expr {
    enum expr_type {
        EXPR_NUMBER,
        EXPR_IDENTIFIER,
        EXPR_STRING,
        EXPR_BINARY,
    } type;
    union {
        int number;
        char *identifier;
        char *string;
        struct {
            enum expr_binary_type {
                EXPR_BINARY_PLUS,
            } type;
            struct expr *lhs, *rhs;
        } binary;
    };
};

void expr_free(struct expr *expr);
struct expr *expr_copy(struct expr const *expr);
struct expr *expr_number(int number);
struct expr *expr_identifier(char const *identifier);
struct expr *expr_string(char const *identifier);
struct expr *expr_binary(enum expr_binary_type type, struct expr const *lhs, struct expr const *rhs);

struct stmt {
    enum stmt_type {
        STMT_ASSIGN,
        STMT_PRINT,
    } type;
    union {
        struct {
            char *identifier;
            struct expr *value;
        } assign;
        struct expr *print;
    };
};

struct stmt *stmt_assign(char const *identifier, struct expr const *expr);
struct stmt *stmt_print(struct expr const *expr);
void stmt_free(struct stmt *stmt);

struct stmt_list {
    struct stmt *stmt;
    struct stmt_list *next;
};

void stmt_list_append(struct stmt_list **list, struct stmt const *stmt);
void stmt_list_free(struct stmt_list *list);

typedef struct {
    struct stmt_list *stmt_list;
} Program;

int yylex(void);
void yyerror(Program *program, char const *message);

union token {
    struct stmt *stmt;
    struct expr *expr;

    char *identifier;
    char *string;
    int number;
    struct buffer *buf;
};

#endif

/* vim: set sw=4 et: */
