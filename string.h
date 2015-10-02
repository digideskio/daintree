// daintree

#ifndef __STRING_H__
#define __STRING_H__

#include <arch.h>

char *strdup(char const *s1);
char *strndup(char const *s1, uint32_t n);
int strncmp(char const *s1, char const *s2, uint32_t n);
int strlen(char const *s);

struct buffer {
    char *buffer;
    int used, allocated;
};

struct buffer *alloc_buffer(void);
void append_buffer_str(struct buffer *buf, char const *s);
void append_buffer_char(struct buffer *buf, char c);
void free_buffer(struct buffer *buf);

#endif

/* vim: set sw=4 et: */
