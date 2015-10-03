// daintree

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <arch.h>

void scroll(void);
void cursor(void);
void getcursor(int *x, int *y);
void setcursor(int x, int y);
void putc(char c);
void puts(char const *s);
void putn(int n);
int vasputf(char **p, char const *fmt, va_list ap);
void putf(char const *fmt, ...);
void clear(void);
uint8_t readch(void);
char *gets(void);

#endif

// vim: set sw=4 et:
