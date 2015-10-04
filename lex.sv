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

[0-9]+
    context->number = atoi(match);
    return NUMBER;

[ \t]+

"
    BEGIN(string);
    context->buf = alloc_buffer();

\*\*
    return EXP;

None
    return K_NONE;

True
    return K_TRUE;

False
    return K_FALSE;

.
    return match[0];

*mode string

[^"\\]+
    append_buffer_str(context->buf, match);

\\.
    append_buffer_char(context->buf, match[1]);

"
    END();

    char *string = strndup(context->buf->buffer, context->buf->used);
    free_buffer(context->buf);
    context->string = string;
    return STRING;

*# vim: set sw=4 et:
