%{
    #include <ast.h>
%}

%define api.value.type {union token}

%parse-param {Program *program}
%error-verbose

%token END_OF_FILE 0 "$end"
%token NL PRINT LPAREN RPAREN LBRACKET RBRACKET

%type <stmt> stmt line
%type <expr> expr

%left LOGICAL_AND LOGICAL_OR
%left LOGICAL_NOT
%right '=' ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN
%nonassoc ELLIPSIS
%left EQ NE
%left LT LE GE GT
%left BINARY_OR BINARY_XOR
%left BINARY_AND
%left LSHIFT RSHIFT
%left '+' MINUS
%left TIMES DIVIDE MODULO
%right EXP

%nonassoc <identifier> IDENTIFIER
%nonassoc <number> NUMBER
%nonassoc <string> STRING

%destructor { free($$); } <identifier>
%destructor { free($$); } <string>
%destructor { stmt_free($$); } <stmt>
%destructor { expr_free($$); } <expr>

%%

input:
    /* empty */
  | input line { if ($2) { stmt_list_append(&program->stmt_list, $2); stmt_free($2); } }
;

line_separator:
    NL
  | ';'
;

line:
    line_separator      { $$ = 0; }
  | stmt line_separator { $$ = $1; }
  | stmt END_OF_FILE    { $$ = $1; }
;

stmt:
    IDENTIFIER '=' expr { $$ = stmt_assign($1, $3); free($1); expr_free($3); }
  | PRINT expr { $$ = stmt_print($2); expr_free($2); }
;

expr:
    NUMBER { $$ = expr_number($1); }
  | IDENTIFIER { $$ = expr_identifier($1); free($1); }
  | STRING { $$ = expr_string($1); free($1); }
  | expr '+' expr { $$ = expr_binary(EXPR_BINARY_PLUS, $1, $3); expr_free($1); expr_free($3); }
;

/* vim: set sw=4 et: */
