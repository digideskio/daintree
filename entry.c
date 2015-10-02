// daintree

#include <arch.h>
#include <mem.h>
#include <console.h>
#include <multiboot.h>
#include <program.h>
#include <build/parse.tab.h>

struct lexer;
extern struct lexer *active_lexer;
struct lexer *lexer_start_str(char const *str);
void lexer_free(struct lexer *lexer);

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

        lexer_free(active_lexer);

        stmt_list_free(program.stmt_list);

        free(i);

        putf("heap use: %x/%x\n", heap_in_use, heap_size);
    }

    context_free(context);

    asm volatile("hlt");
}

// vim: set sw=4 et:
