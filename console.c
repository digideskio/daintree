// daintree

#include <arch.h>
#include <console.h>
#include <mem.h>

#define CURSOR_IDX 0x3d4
#define CURSOR_DATA 0x3d5
#define CURSOR_MSB_IDX 0xe
#define CURSOR_LSB_IDX 0xf

static uint8_t *vmem = (uint8_t *)0xb8000;

static uint8_t vx = 0, vy = 0;

void scroll() {
    while (vx >= 80) {
        vx -= 80;
        ++vy;
    }

    while (vy >= 25) {
        for (int i = 0; i < 80 * 24; ++i) {
            vmem[i * 2] = vmem[i * 2 + 160];
            vmem[i * 2 + 1] = vmem[i * 2 + 161];
        }
        for (int i = 80 * 24; i < 80 * 25; ++i) {
            vmem[i * 2] = ' ';
            vmem[i * 2 + 1] = 0x07;
        }
        --vy;
    }
}

void cursor(void) {
    uint16_t idx = vy * 80 + vx;
    out8(CURSOR_IDX, CURSOR_MSB_IDX);
    out8(CURSOR_DATA, (idx >> 8) & 0xff);
    out8(CURSOR_IDX, CURSOR_LSB_IDX);
    out8(CURSOR_DATA, idx & 0xff);
}

void putc(char c) {
    if (c == '\n') {
        vx = 0;
        ++vy;
    } else if (c == '\t') {
        vx += 8 - (vx % 8);
    } else {
        vmem[(vy * 80 + vx) * 2] = c;
        vmem[(vy * 80 + vx) * 2 + 1] = 0x07;
        ++vx;
    }
    scroll();
    cursor();
}

void puts(char const *s) {
    char c;
    while ((c = *s++)) {
        putc(c);
    }
}

void putn(int n) {
    int orig = n;
    int base = 10, index = 1, digits = 1;
    
    while (n / index >= base) {
        index *= base;
        ++digits;
    }

    do {
        int c = n / index;
        n -= c * index;

        if (!c && index > orig && index != 1) {
            putc('0');
        } else {
            putc(c + '0');
        }

        index /= base;
        --digits;
    } while (index >= 1);
}

void vaputf(char **p, char const *fmt, va_list ap) {
    // needs malloc
}

void putf(char const *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *p;
    vaputf(&p, fmt, ap);
    puts(p);
    free(p);
}

void clear(void) {
    for (int i = 0; i < 80 * 25; ++i) {
        vmem[i * 2] = ' ';
        vmem[i * 2 + 1] = 0x07;
    }
    vx = vy = 0;
    cursor();
}

char readch(void) {
    while (1) {
        uint8_t status = in8(0x64);
        if ((status & 0x01) == 0x01) {
            break;
        }
    }

    return in8(0x60);
}

// vim: set sw=4 et:
