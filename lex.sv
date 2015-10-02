*raw
    #include <program.h>
    #include <parse.tab.h>
    #include <mem.h>
    #include <console.h>

    struct lexer *lexer_start_str(char const *src);
    int lexer_lex(struct lexer *lexer, union token *context);

    struct lexer *active_lexer;
    union token yylval;

    int yylex(void) {
        if (!active_lexer) {
            return 0;
        }

        return lexer_lex(active_lexer, &yylval);
    }

    void yyerror(Program *program, char const *message) {
        putf("yyerror: %s\n", message);
    }

*set context = union token

[a-z]+
    context->identifier = strdup(match);
    return IDENTIFIER;

=
    return EQUALS;

[0-9]+
    context->number = atoi(match);
    return NUMBER;

[ \t]+

*# vim: set sw=4 et:
