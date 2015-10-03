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

static char keyboard_us[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',              /* 0~9 */
    '9', '0', '-', '=', '\b',                                   /* ~14 */
    '\t', 'q', 'w', 'e', 'r',                                   /* ~19 */
    't', 'y', 'u', 'i', 'o', 'p',                               /* ~25 */
    '[', ']', '\n', 0,                                          /* ~29 (CTRL) */
    'a', 's', 'd', 'f', 'g', 'h',                               /* ~35 */
    'j', 'k', 'l', ';', '\'',                                   /* ~40 */
    '`', 0, '\\', 'z', 'x', 'c',                                /* ~46 (LSHFT) */
    'v', 'b', 'n', 'm', ',',                                    /* ~51 */
    '.', '/', 0, '*',                                           /* ~55 (RSHFT) */
    0, ' ', 0, 0, 0, 0, 0, 0, 0,                                /* ~64 (ALT, CAPSL, F1~6) */
    0, 0, 0, 0,                                                 /* ~68 (F7~10) */
    0, 0, 0, 0, 0, '-',                                         /* ~74 (NUML, SCROLLL, HOME, UP, PGUP) */
    0, 0, 0, '+',                                               /* ~78 (LEFT, ?, RIGHT) */
    0, 0, 0, 0, 0,                                              /* ~83 (END, DOWN, PGDN, INS, DEL) */
    0, 0, 0, 0, 0,                                              /* ~88 (?, ?, ?, F11~12) */
    0                                                           /* undefined */
};

static char keyboard_dv[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',              /* 0~9 */
    '9', '0', '[', ']', '\b',                                   /* ~14 */
    '\t', '\'', ',', '.', 'p',                                  /* ~19 */
    'y', 'f', 'g', 'c', 'r', 'l',                               /* ~25 */
    '/', '=', '\n', 0,                                          /* ~29 (CTRL) */
    'a', 'o', 'e', 'u', 'i', 'd',                               /* ~35 */
    'h', 't', 'n', 's', '-',                                    /* ~40 */
    '`', 0, '\\', ';', 'q', 'j',                                /* ~46 (LSHFT) */
    'k', 'x', 'b', 'm', 'w',                                    /* ~51 */
    'v', 'z', 0, '*',                                           /* ~55 (RSHFT) */
    0, ' ', 0, 0, 0, 0, 0, 0, 0,                                /* ~64 (ALT, CAPSL, F1~6) */
    0, 0, 0, 0,                                                 /* ~68 (F7~10) */
    0, 0, 0, 0, 0, '-',                                         /* ~74 (NUML, SCROLLL, HOME, UP, PGUP) */
    0, 0, 0, '+',                                               /* ~78 (LEFT, ?, RIGHT) */
    0, 0, 0, 0, 0,                                              /* ~83 (END, DOWN, PGDN, INS, DEL) */
    0, 0, 0, 0, 0,                                              /* ~88 (?, ?, ?, F11~12) */
    0                                                           /* undefined */
};

/* Table of characters and their shifted equivalents. */
static char keyboard_us_shift_table[] = {
    '`', '~',   '1', '!',   '2', '@',   '3', '#',
    '4', '$',   '5', '%',   '6', '^',   '7', '&',
    '8', '*',   '9', '(',   '0', ')',   '-', '_',
    '=', '+',   '[', '{',   ']', '}',   '\\', '|',
    ';', ':',   '\'', '"',  ',', '<',   '.', '>',
    '/', '?',
    0
};

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

static void wait_kb(void) {
    while (1) {
        if ((in8(0x64) & 0x02) == 0) {
            break;
        }
    }
}

static int capslock = 0;

static char shift_ch(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        return ch - 0x20;
    }

    char *p = keyboard_us_shift_table;
    while (*p) {
        if (p[0] == ch) {
            return p[1];
        }
        p += 2;
    }
    return ch;
}

static char capslock_ch(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        return ch - 0x20;
    }

    if (ch >= 'A' && ch <= 'Z') {
        return ch + 0x20;
    }

    return ch;
}

char *gets(void) {
    struct buffer *buf = alloc_buffer();
    int ctrl = 0, shift = 0, alt = 0;

    while (1) {
        int update_leds = 0;
        current_task->task->waiting_irq = 1;

        uint32_t ch32;
        __asm__ __volatile__("int $0x80" : "=a" (ch32) : "0" (1));
        uint8_t ch = in8(0x60);

        if (ch & 0x80) {
            ch &= ~0x80;

            switch (ch) {
            case 29:
                ctrl = 0;
                break;
            case 42:
            case 54:
                shift = 0;
                break;
            case 56:
                alt = 0;
                break;
            }
        } else {
            switch (ch) {
            case 29:
                ctrl = 1;
                break;
            case 42:
            case 54:
                shift = 1;
                break;
            case 56:
                alt = 1;
                break;
            case 58:
                capslock = !capslock;
                update_leds = 1;
                break;
            default:
                ch = keyboard_dv[ch];

                if (shift) {
                    ch = shift_ch(ch);
                }

                if (capslock) {
                    ch = capslock_ch(ch);
                }

                if (ch == '\b') {
                    if (buf->used) {
                        putc(ch);
                        --buf->used;
                    }
                } else if (ch == '\n') {
                    putc(ch);
                    goto done;
                } else {
                    putc(ch);
                    append_buffer_char(buf, ch);
                }
                break;
            }
        }

        if (update_leds) {
            uint8_t value = capslock ? 0x04 : 0;
            wait_kb();
            out8(0x60, 0xed);
            wait_kb();
            out8(0x60, value);
        }
    }

done: { }
    char *result = strndup(buf->buffer, buf->used);
    free_buffer(buf);
    return result;
}

// vim: set sw=4 et:
