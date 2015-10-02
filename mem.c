// daintree

#include <mem.h>

uint32_t placement_malloc;

void *malloc(uint32_t n) {
    if (placement_malloc % 4 != 0) {
        placement_malloc += 4 - (placement_malloc % 4);
    }

    void *r = (void *)placement_malloc;
    placement_malloc += n;
    return r;
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

// vim: set sw=4 et:
