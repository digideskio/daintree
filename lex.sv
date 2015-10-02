*raw
    #include <program.h>
    #include <parse.tab.h>
    #include <mem.h>
    #include <console.h>

    int yylex(void) {
        return 1;
    }

    void yyerror(Program *program, char const *message) {
        puts(message);
    }

[a-z]+
    return IDENTIFIER;

*# vim: set sw=4 et:
