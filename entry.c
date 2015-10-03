// daintree

#include <arch.h>
#include <mem.h>
#include <console.h>
#include <multiboot.h>
#include <program.h>
#include <interrupts.h>
#include <build/parse.tab.h>

struct lexer;
extern struct lexer *active_lexer;
struct lexer *lexer_start_str(char const *str);
void lexer_free(struct lexer *lexer);

static void entry_continue(void);

void entry(multiboot_info_t *multiboot) {
    if (multiboot == 0) {
        puts("no multiboot info. halting.");
        return;
    }

    heap_init((uint32_t) &_kend, 0x100000 + (multiboot->mem_upper * 1024));
    void *stack = malloc(0x2000);
    __asm__ __volatile__("\
        mov %%cr3, %%eax; \
        mov %%eax, %%cr3" : : : "%eax");
    __asm__ __volatile__("\
        mov %%eax, %%esp; \
        mov %%eax, %%ebp" : : "a" ((uint32_t) stack));

    entry_continue();
}

void entry_continue(void) {
    clear();
    puts("daintree\n");

    interrupts_init();

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
    }

    context_free(context);

    asm volatile("hlt");
}

// vim: set sw=4 et:
