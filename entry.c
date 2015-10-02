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

    Context *context = context_new();
    
    while (1) {
        puts("> ");
        char *i = gets();

        Program program;
        memset(&program, 0, sizeof(program));
        active_lexer = lexer_start_str(i);
        int r = yyparse(&program);
        if (!r) {
            program_run(&program, context);
        }

        free(i);
    }

    context_free(context);

    asm volatile("hlt");
}

// vim: set sw=4 et:
