// daintree

#include <arch.h>
#include <mem.h>
#include <console.h>
#include <multiboot.h>
#include <program.h>
#include <build/parse.tab.h>

void entry(multiboot_info_t *multiboot) {
    if (multiboot == 0) {
        puts("no multiboot info. halting.");
        return;
    }

    placement_malloc = (uint32_t) &_kend;

    clear();
    puts("daintree\n");

    Program program;
    program.id = 0;

    int r = yyparse(&program);
    putf("yyparse: %d\n", r);
    putf("id: %x %s\n", program.id, program.id);

    asm volatile("hlt");
}

// vim: set sw=4 et:
