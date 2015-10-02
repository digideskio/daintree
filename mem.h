// daintree

#ifndef __MEM_H__
#define __MEM_H__

#include <arch.h>

void heap_init(uint32_t lower, uint32_t upper);
void *malloc(uint32_t n);
void free(void *p);
void *memcpy(void *dst, void const *src, uint32_t n);
void *memset(void *dst, int c, uint32_t len);

#endif

// vim: set sw=4 et:
