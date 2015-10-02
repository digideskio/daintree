// daintree

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

typedef struct {
    char *id;
} Program;

union token {
    char *identifier;
};

int yylex(void);
void yyerror(Program *program, char const *message);

#endif

/* vim: set sw=4 et: */
