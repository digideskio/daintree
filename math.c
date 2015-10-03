// daintree

#include <math.h>

int powi(int x, int y) {
    if (y <= 0) {
        return 1;
    }
    int r = x;
    while (--y) {
        r *= x;
    }
    return r;
}

/* vim: set sw=4 et: */
