// daintree

#ifndef __STRING_H__
#define __STRING_H__

#include <arch.h>

char *strndup(char const *s1, uint32_t n);
int strncmp(char const *s1, char const *s2, uint32_t n);
int strlen(char const *s);

#endif

/* vim: set sw=4 et: */
