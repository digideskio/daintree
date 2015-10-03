// daintree 

#include <arch.h>
#include <console.h>
#include <interrupts.h>
#include <mem.h>
#include <task.h>

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    union {
        uint8_t access;
        __extension__ struct {
            unsigned segment_type : 4;
            unsigned descriptor_type : 1;
            unsigned privilege_level : 2;
            unsigned present : 1;
        } __attribute__((__packed__));
    };
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((__packed__));

struct gdt_pointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((__packed__));

static struct gdt_entry gdt_entries[4];
static struct gdt_pointer gdt_pointer;

static char const *isr_messages[] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Into detected overflow",
    "Out of bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    /* reserved exceptions are empty */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

union idt_entry {
    __extension__ struct {
        unsigned offset_low : 16;
        unsigned selector : 16;
        unsigned _always_0 : 8;
        unsigned flags : 8;
        unsigned offset_high : 16;
    } __attribute__((__packed__));
    unsigned long ulong;
} __attribute__((__packed__));

struct idt_pointer {
    uint16_t limit;
    uint32_t ridt;
} __attribute__((__packed__));

static union idt_entry idt_entries[256];
static struct idt_pointer idt_pointer;

static void set_gdt_gate_fields(uint8_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt_entries[num].base_low = base & 0xffff;
    gdt_entries[num].base_middle = (base >> 16) & 0xff;
    gdt_entries[num].base_high = (base >> 24) & 0xff;
    gdt_entries[num].limit_low = limit & 0xffff;
    // We throw away half of granularity here and a quarter of limit and it's probably incorrect.
    gdt_entries[num].granularity = ((limit >> 16) & 0x0f) | (granularity & 0xf0);
    gdt_entries[num].access = access;
}

static void set_gdt_gate(uint8_t num, uint32_t base, uint32_t limit, uint8_t dpl, int code) {
    set_gdt_gate_fields(num, base, limit, (code ? 0xa : 0x2) | (dpl << 5) | (0x9 << 4), 0xcf);
}

static void set_idt_gate(uint8_t idt, void (*callback)(), uint16_t isr_segment, uint8_t flags) {
    idt_entries[idt].offset_low = (uint32_t) callback & 0xffff;
    idt_entries[idt].selector = isr_segment;
    idt_entries[idt].flags = flags;
    idt_entries[idt].offset_high = ((uint32_t) callback >> 16) & 0xffff;
}

void interrupts_init(void) {
    gdt_pointer.limit = sizeof(gdt_entries) - 1;
    gdt_pointer.base = (uint32_t) gdt_entries;

    memset(&gdt_entries, 0, sizeof(gdt_entries));
    set_gdt_gate(1, 0, 0xffffffff, 0, 1);
    set_gdt_gate(2, 0, 0xffffffff, 0, 0);

    memset(&idt_entries, 0, sizeof(idt_entries));

#define SET_IDT_GATE(n) set_idt_gate(0x##n, isr##n, 0x08, 0x8e)
    SET_IDT_GATE(0); SET_IDT_GATE(1); SET_IDT_GATE(2); SET_IDT_GATE(3);
    SET_IDT_GATE(4); SET_IDT_GATE(5); SET_IDT_GATE(6); SET_IDT_GATE(7);
    SET_IDT_GATE(8); SET_IDT_GATE(9); SET_IDT_GATE(a); SET_IDT_GATE(b);
    SET_IDT_GATE(c); SET_IDT_GATE(d); SET_IDT_GATE(e); SET_IDT_GATE(f);
    SET_IDT_GATE(10); SET_IDT_GATE(11); SET_IDT_GATE(12); SET_IDT_GATE(13);
    SET_IDT_GATE(14); SET_IDT_GATE(15); SET_IDT_GATE(16); SET_IDT_GATE(17);
    SET_IDT_GATE(18); SET_IDT_GATE(19); SET_IDT_GATE(1a); SET_IDT_GATE(1b);
    SET_IDT_GATE(1c); SET_IDT_GATE(1d); SET_IDT_GATE(1e); SET_IDT_GATE(1f);
    SET_IDT_GATE(80);
#undef SET_IDT_GATE

    idt_pointer.limit = sizeof(idt_entries) - 1;
    idt_pointer.ridt = (uint32_t) idt_entries;

    __asm__ __volatile__("lidt %0" : : "m" (idt_pointer));

    out8(0x20, 0x11); out8(0xa0, 0x11);
    // mapping to 0x20/0x28 here
    out8(0x21, 0x20); out8(0xa1, 0x28);
    out8(0x21, 0x04); out8(0xa1, 0x02);
    out8(0x21, 0x01); out8(0xa1, 0x01);
    out8(0x21, 0x00); out8(0xa1, 0x00);

#define SET_IRQ_GATE(n) set_idt_gate(0x2##n, irq##n, 0x08, 0x8e)
    SET_IRQ_GATE(0); SET_IRQ_GATE(1); SET_IRQ_GATE(2); SET_IRQ_GATE(3);
    SET_IRQ_GATE(4); SET_IRQ_GATE(5); SET_IRQ_GATE(6); SET_IRQ_GATE(7);
    SET_IRQ_GATE(8); SET_IRQ_GATE(9); SET_IRQ_GATE(a); SET_IRQ_GATE(b);
    SET_IRQ_GATE(c); SET_IRQ_GATE(d); SET_IRQ_GATE(e); SET_IRQ_GATE(f);
#undef SET_IRQ_GATE

    uint16_t timer = 0x1234dc / 100;
    out8(0x43, 0x36);
    out8(0x40, timer & 0xff);
    out8(0x40, (timer >> 8) & 0xff);

    __asm__ __volatile__("sti");
}

void *isr_handler(struct callback_registers *r) {
    int int_no = r->int_no;
    putf("exception %d: %s\n", int_no, isr_messages[int_no] ? isr_messages[int_no] : "reserved");
    __asm__ __volatile__("hlt");
    return r;
}

void *irq_handler(struct callback_registers *r) {
    if (r->int_no >= 0x28) {
        out8(0xa0, 0x20);
    }
    out8(0x20, 0x20);

    if (r->int_no == 0x20) {
        return tasks_switch(r);
    }

    return r;
}

// vim: set sw=4 et:
