// daintree

#ifndef __ARCH_H__
#define __ARCH_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef uint32_t *va_list;
#define va_start(ap, p) (ap = (uint32_t *)&p)
#define va_arg(ap, t) ((t)*(++(ap)))

void out8(uint16_t port, uint8_t data);

#endif

// vim: set sw=2 cc=80 et:
