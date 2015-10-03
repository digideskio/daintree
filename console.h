// daintree

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <arch.h>

void scroll(void);
void cursor(void);
void putc_at(uint8_t *vx, uint8_t *vy, char c);
void putc(char c);
void puts_at(uint8_t *vx, uint8_t *vy, char const *s);
void puts(char const *s);
void putn(int n);
int vasputf(char **p, char const *fmt, va_list ap);
void putf_at(uint8_t *vx, uint8_t *vy, char const *fmt, ...);
void putf(char const *fmt, ...);
char *sputf(char const *fmt, ...);
void clear(void);
uint8_t readch(void);
char *gets(void);

#endif

// vim: set sw=4 et:
