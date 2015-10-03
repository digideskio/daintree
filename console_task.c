// daintree

#include <arch.h>
#include <console.h>
#include <mem.h>
#include <string.h>
#include <task.h>

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

static int capslock = 0;

static void wait_kb(void);
static char shift_ch(char ch);
static char capslock_ch(char ch);

struct buffer *console_task_buf;

void console_task(void) {
    console_task_buf = alloc_buffer();

    int ctrl = 0, shift = 0, alt = 0;

    uint32_t r;
    __asm__ __volatile__("int $0x80" : "=a" (r) : "0" (1), "b" (1));

    while (1) {
        int update_leds = 0;

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
                    if (console_task_buf->used) {
                        putc(ch);
                        --console_task_buf->used;
                    }
                } else if (ch == '\n') {
                    putc(ch);
                    __asm__ __volatile__("int $0x80" : "=a" (r) : "0" (3));
                    console_task_buf->used = 0;
                } else {
                    putc(ch);
                    append_buffer_char(console_task_buf, ch);
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

        __asm__ __volatile__("int $0x80" : "=a" (r) : "0" (2));
    }

}

static void wait_kb(void) {
    while (1) {
        if ((in8(0x64) & 0x02) == 0) {
            break;
        }
    }
}

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

// vim: set sw=4 et:
