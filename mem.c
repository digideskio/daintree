// daintree

#include <mem.h>
#include <console.h>

// I'm keeping this struct's size modulo 4 to make later maths easier.
struct heap_entry {
    struct heap_entry *prev, *next;
    uint32_t size;
    uint32_t free;
} *heap;

uint32_t heap_size, heap_in_use;

void heap_init(uint32_t lower, uint32_t upper) {
    // Align the bottom of the heap.
    if (lower % 4 != 0) {
        lower += 4 - (lower % 4);
    }

    heap = (struct heap_entry *)lower;
    heap->prev = heap->next = NULL;
    heap_size = heap->size = upper - lower - sizeof(*heap);
    heap->free = 1;

    heap_in_use = 0;
}

void *malloc(uint32_t n) {
    if (!n) {
        return NULL;
    }

    // Keeping everything aligned ...
    if (n % 4 != 0) {
        n += 4 - (n % 4);
    }

    struct heap_entry *search = heap;
    while (search && !(search->free && search->size >= n)) {
        search = search->next;
    }

    if (!search) {
        puts("\nOOM!\n");
        return NULL;
    }

    search->free = 0;
    if (search->size - n <= sizeof(struct heap_entry) + 4) {
        // No room for even one extra allocation, so screw that.
        heap_in_use += search->size;
        return (void *)search + sizeof(struct heap_entry);
    }

    // okay let's do this
    struct heap_entry *leftover = ((void *)search + sizeof(struct heap_entry) + n);
    leftover->prev = search;
    leftover->next = search->next;
    search->next = leftover;
    leftover->next->prev = leftover;
    leftover->size = search->size - n - sizeof(struct heap_entry);
    leftover->free = 1;

    search->size = n;
    heap_in_use += sizeof(struct heap_entry) + n;
    return (void *)search + sizeof(struct heap_entry);
}

void free(void *p) {
    if (!p) {
        return;
    }

    struct heap_entry *entry = (p - sizeof(struct heap_entry));
    if (entry->free) {
        puts("double free!\n");
        return;
    }

    entry->free = 1;

    heap_in_use -= entry->size;

    while (entry->prev && entry->prev->free) {
        entry->prev->size += sizeof(struct heap_entry) + entry->size;
        entry->prev->next = entry->next;
        entry = entry->prev;
        if (entry->next) {
            entry->next->prev = entry;
        }
        heap_in_use -= sizeof(struct heap_entry);
    }

    while (entry->next && entry->next->free) {
        entry->size += sizeof(struct heap_entry) + entry->next->size;
        if (entry->next->next) {
            entry->next->next->prev = entry;
        }
        entry->next = entry->next->next;
        heap_in_use -= sizeof(struct heap_entry);
    }
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
