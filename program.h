// daintree

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

struct lexer;
extern struct lexer *active_lexer;
struct lexer *lexer_start_str(char const *str);

struct stmt {
    char *identifier;
    int number;
};

struct stmt *stmt_new(char const *ident, int num);

typedef struct {
    struct stmt *stmt;
} Program;


union token {
    struct stmt *stmt;

    char *identifier;
    int number;
};

int yylex(void);
void yyerror(Program *program, char const *message);

#endif

/* vim: set sw=4 et: */
