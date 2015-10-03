// daintree

#include <arch.h>
#include <console.h>
#include <mem.h>
#include <string.h>
#include <task.h>

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

void getcursor(int *x, int *y) {
    *x = vx; *y = vy;
}

void setcursor(int x, int y) {
    vx = x; vy = y;
}

void putc(char c) {
    if (c == '\n') {
        vx = 0;
        ++vy;
    } else if (c == '\t') {
        vx += 8 - (vx % 8);
    } else if (c == '\b') {
        if (!vx) {
            if (vy) {
                --vy;
                vx = 79;
            }
        } else {
            --vx;
        }
        vmem[(vy * 80 + vx) * 2] = ' ';
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

struct buffer *alloc_buffer(void) {
    struct buffer *buf = malloc(sizeof(*buf));
    buf->buffer = malloc(8);
    buf->used = 0;
    buf->allocated = 8;
    return buf;
}

static void size_buffer(struct buffer *buf, int req) {
    int allocated = buf->allocated;
    while (allocated - buf->used < req) {
        allocated *= 2;
    }

    if (allocated == buf->allocated) {
        return;
    }

    char *newbuffer = malloc(allocated);
    memcpy(newbuffer, buf->buffer, buf->used);
    free(buf->buffer);
    buf->buffer = newbuffer;
    buf->allocated = allocated;
}

void append_buffer_str(struct buffer *buf, char const *s) {
    int len = strlen(s);
    size_buffer(buf, len);
    memcpy(buf->buffer + buf->used, s, len);
    buf->used += len;
};

void append_buffer_char(struct buffer *buf, char c) {
    size_buffer(buf, 1);
    buf->buffer[buf->used++] = c;
}

void free_buffer(struct buffer *buf) {
    free(buf->buffer);
    free(buf);
}

int vasputf(char **p, char const *fmt, va_list ap) {
    struct buffer *buf = alloc_buffer();

    int is_escape = 0;
    int lenmod;
    char c, padder, sepr;

    while ((c = *fmt++)) {
        if (is_escape) {
            if (c == '%') {
                append_buffer_char(buf, c);
                is_escape = 0;
                continue;
            }

            if ((c == ' ' || c == '0') && !lenmod && padder == 0) {
                padder = c;
                continue;
            }

            if (c == ',') {
                sepr = c;
                continue;
            }

            if (c >= '0' && c <= '9') {
                lenmod = (lenmod * 10) + (c - '0');
                continue;
            }

            switch (c) {
            case 's':
                append_buffer_str(buf, va_arg(ap, char const *));
                break;

            case 'd':
            case 'x':
                {
                    int n = va_arg(ap, int);
                    int orig = n;
                    int base = (c == 'd') ? 10 : 16;

                    int index = 1, digits = 1, separator;

                    switch (base) {
                    case 10: separator = 3; break;
                    case 2: case 8: case 16: separator = 4; break;
                    }

                    while (n / index >= base || digits < lenmod) {
                        index *= base;
                        ++digits;
                    }

                    do {
                        int c = (n / index);
                        n -= c * index;

                        if (!c && index > orig && index != 1) {
                            append_buffer_char(buf, padder == 0 ? ' ' : padder);
                        } else {
                            append_buffer_char(buf, (c >= 0 && c <= 9) ? (c + '0') : (c - 10 + 'a'));
                        }

                        index /= base;
                        --digits;

                        if (sepr && separator > 0 && digits && separator == 0 && index >= 1) {
                            append_buffer_char(buf, sepr);
                        }
                    } while (index >= 1);

                    break;
                }
            }

            is_escape = 0;
        } else if (c == '%') {
            is_escape = 1;
            lenmod = padder = sepr = 0;
        } else {
            append_buffer_char(buf, c);
        }
    }

    int used = buf->used;
    *p = strndup(buf->buffer, used);
    free_buffer(buf);
    return used;
}

void putf(char const *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *p;
    vasputf(&p, fmt, ap);
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

uint8_t readch(void) {
    while (1) {
        if (in8(0x64) & 0x01) {
            break;
        }
    }

    return in8(0x60);
}

char *gets(void) {
    uint32_t r;
    __asm__ __volatile__("int $0x80" : "=a" (r) : "0" (4));
    return (char *)r;
}

// vim: set sw=4 et:
