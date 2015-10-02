// daintree

#include <mem.h>

void *malloc(uint32_t n) {
  return (void *)0;
}

void free(void *p) {
  // no-op (until alloc)
}

void *memcpy(void *dst, void const *src, uint32_t n) {
  char *cdst = dst;
  char const *csrc = src;
  while (n--) {
    *cdst++ = *csrc++;
  }
  return dst;
}

void *memset(void *dst, int c, uint32_t len) {
  char *cdst = dst;
  while (len--) {
    *cdst++ = c;
  }
  return dst;
}

// vim: set sw=2 cc=80 et:
