*raw
    #include <ast.h>
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

print
    return PRINT;

[a-z]+
    context->identifier = strdup(match);
    return IDENTIFIER;

=
    return EQUALS;

\+
    return PLUS;

[0-9]+
    context->number = atoi(match);
    return NUMBER;

[ \t]+

"[^"]*"
    int len = strlen(match);
    context->string = strndup(match + 1, len - 2);
    return STRING;

*# vim: set sw=4 et:
