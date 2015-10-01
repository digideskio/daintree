#ifndef PROGRAM_H
#define PROGRAM_H

typedef struct {

} Program;

union token {
    int stmt;
    int identifier;
};

#endif

/* vim: set sw=4 et: */
