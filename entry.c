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

    heap_init((uint32_t) &_kend, 0x100000 + (multiboot->mem_upper * 1024));

    clear();
    puts("daintree\n");

    Context *context = context_new();
    
    while (1) {
        puts("> ");
        char *i = gets();

        Program program;
        memset(&program, 0, sizeof(program));
        active_lexer = lexer_start_str(i);
        if (!active_lexer) {
            putf("lexer didn't init\n");
            break;
        }

        int r = yyparse(&program);
        if (!r) {
            program_run(&program, context);
        } else {
            putf("parse error\n");
        }

        free(i);
    }

    context_free(context);

    asm volatile("hlt");
}

// vim: set sw=4 et:
