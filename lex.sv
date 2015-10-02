*raw
    #include <program.h>
    #include <parse.tab.h>
    #include <mem.h>
    #include <console.h>

    int yylex(void) {
        return 1;
    }

    void yyerror(Program *program, char const *message) {
        putf("yyerror: %s\n", message);
    }

*set context = union token

[a-z]+
    return IDENTIFIER;

*# vim: set sw=4 et:
