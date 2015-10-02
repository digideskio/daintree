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
    
    while (1) {
        puts("> ");
        char *i = gets();

        Program program;
        memset(&program, 0, sizeof(program));
        active_lexer = lexer_start_str(i);
        int r = yyparse(&program);
        putf("yyparse: %d\n", r);
        putf("stmt: %x\n", program.stmt);
        free(i);
    }

    asm volatile("hlt");
}

// vim: set sw=4 et:
