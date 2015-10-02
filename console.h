// daintree

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <arch.h>

void scroll(void);
void cursor(void);
void putc(char c);
void puts(char const *s);
void putn(int n);
void vaputf(char **p, char const *fmt, va_list ap);
void putf(char const *fmt, ...);
void clear(void);
char readch(void);

#endif

// vim: set sw=4 et:
