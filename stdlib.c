// daintree

#include <stdlib.h>

int atoi(char const *str) {
    int r = 0;
    while (*str) {
        r = (r * 10) + (*str - '0');
        ++str;
    }
    return r;
}

/* vim: set sw=4 et: */
