// daintree

#include <arch.h>
#include <console.h>
#include <multiboot.h>

void entry(multiboot_info_t *multiboot) {
    if (multiboot == 0) {
        puts("no multiboot info. halting.");
        return;
    }

    clear();
    puts("daintree\n");

    asm volatile("hlt");
}

// vim: set sw=4 et:
