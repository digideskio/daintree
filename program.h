// daintree

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

typedef struct {

} Program;

union token {
    int stmt;
    int identifier;
};

int yylex(void);
void yyerror(Program *program, char const *message);

#endif

/* vim: set sw=4 et: */
