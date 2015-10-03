// daintree

#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

struct callback_registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
} __attribute__((__packed__));

struct modeswitch_registers {
    struct callback_registers callback;
    uint32_t useresp, ss;
} __attribute__((__packed__));

void interrupts_init(void);

void isr0(void);
void isr1(void);
void isr2(void);
void isr3(void);
void isr4(void);
void isr5(void);
void isr6(void);
void isr7(void);
void isr8(void);
void isr9(void);
void isra(void);
void isrb(void);
void isrc(void);
void isrd(void);
void isre(void);
void isrf(void);
void isr10(void);
void isr11(void);
void isr12(void);
void isr13(void);
void isr14(void);
void isr15(void);
void isr16(void);
void isr17(void);
void isr18(void);
void isr19(void);
void isr1a(void);
void isr1b(void);
void isr1c(void);
void isr1d(void);
void isr1e(void);
void isr1f(void);
void isr80(void);

void irq0(void);
void irq1(void);
void irq2(void);
void irq3(void);
void irq4(void);
void irq5(void);
void irq6(void);
void irq7(void);
void irq8(void);
void irq9(void);
void irqa(void);
void irqb(void);
void irqc(void);
void irqd(void);
void irqe(void);
void irqf(void);

#endif

// vim: set sw=4 et:
