*raw
    #include <program.h>
    #include <parse.tab.h>
    #include <mem.h>
    #include <console.h>

    struct lexer *lexer_start_str(char const *src);
    int lexer_lex(struct lexer *lexer, union token *context);

    static struct lexer *active_lexer = NULL;
    union token yylval;

    int yylex(void) {
        if (!active_lexer) {
            active_lexer = lexer_start_str("hello");
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

*# vim: set sw=4 et:
