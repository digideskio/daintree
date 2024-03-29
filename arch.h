// daintree

#ifndef __ARCH_H__
#define __ARCH_H__

#define NULL ((void *)0)

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef uint32_t *va_list;
#define va_start(ap, p) (ap = (uint32_t *)&p)
#define va_arg(ap, t) ((t)*(++(ap)))

uint8_t in8(uint16_t port);
void out8(uint16_t port, uint8_t data);

extern void *_kstart, *_kend;

#endif

// vim: set sw=4 et:
