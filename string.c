// daintree

#include <string.h>
#include <mem.h>

char *strdup(char const *s1) {
    int len = strlen(s1);
    char *dst = malloc(len + 1);
    memcpy(dst, s1, len + 1);
    return dst;
}
char *strndup(char const *s1, uint32_t n) {
    char *dst = malloc(n + 1);
    char *w = dst;

    while (n--) {
        *w = *s1++;
        if (!*w) {
            return dst;
        }
        ++w;
    }

    *w = 0;

    return dst;
}

int strncmp(char const *s1, char const *s2, uint32_t n) {
    while (n--) {
        if (*s1 < *s2) {
            return -1;
        } else if (*s1 > *s2) {
            return 1;
        } else if (!*s1) {
            return 0;
        }
    }
    return 0;
}

int strlen(char const *s) {
    int n = 0;
    while (*s++) {
        ++n;
    }
    return n;
}

/* vim: set sw=4 et: */
