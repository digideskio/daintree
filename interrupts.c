// daintree 

#include <arch.h>
#include <console.h>
#include <interrupts.h>
#include <mem.h>

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

static void set_idt_gate(uint8_t idt, void (*callback)(), uint16_t isr_segment, uint8_t flags) {
    idt_entries[idt].offset_low = (uint32_t) callback & 0xFFFF;
    idt_entries[idt].selector = isr_segment;
    idt_entries[idt].flags = flags | 0x60;
    idt_entries[idt].offset_high = ((uint32_t) callback >> 16) & 0xFFFF;
}

void interrupts_init(void) {
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
}

void *isr_handler(struct modeswitch_registers *r) {
    int int_no = r->callback.int_no;
    putf("exception %d: %s\n", int_no, isr_messages[int_no] ? isr_messages[int_no] : "reserved");
    return r;
}

void *irq_handler(struct modeswitch_registers *r) {
    if (r->callback.int_no >= 0x28) {
        out8(0xa0, 0x20);
    }
    out8(0x20, 0x20);
    return r;
}

// vim: set sw=4 et:
