// daintree

#include <arch.h>
#include <console.h>
#include <multiboot.h>
#include <program.h>
#include <build/parse.tab.h>

void entry(multiboot_info_t *multiboot) {
    if (multiboot == 0) {
        puts("no multiboot info. halting.");
        return;
    }

    clear();
    puts("daintree\n");

    Program program;
    int r = yyparse(&program);
    putf("yyparse: %d\n", r);

    asm volatile("hlt");
}

// vim: set sw=4 et:
