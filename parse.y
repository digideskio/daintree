%{
    #include <ast.h>
    #include <arch.h>
%}

%define api.value.type {union token}

%parse-param {Program *program}
%error-verbose

%token END_OF_FILE 0 "$end"
%token NL PRINT LPAREN RPAREN LBRACKET RBRACKET

%type <stmt> stmt line
%type <expr> expr
%type <exprlist> exprlist opt_exprlist
%type <exprlist> dictlist opt_dictlist

%left LOGICAL_AND LOGICAL_OR
%left LOGICAL_NOT
%right '=' ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN
%nonassoc ELLIPSIS
%left EQ NE
%left LT LE GE GT
%left BINARY_OR BINARY_XOR
%left BINARY_AND
%left LSHIFT RSHIFT
%left '+' '-'
%left '*' '/' '%'
%left NEG
%right EXP

%nonassoc <identifier> IDENTIFIER
%nonassoc <number> NUMBER
%nonassoc <string> STRING

%destructor { free($$); } <identifier>
%destructor { free($$); } <string>
%destructor { stmt_free($$); } <stmt>
%destructor { expr_free($$); } <expr>
%destructor { expr_list_free($$); } <exprlist>

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
  | expr { $$ = stmt_print($1); expr_free($1); }
;

expr:
    NUMBER { $$ = expr_number($1); }
  | IDENTIFIER { $$ = expr_identifier($1); free($1); }
  | STRING { $$ = expr_string($1); free($1); }
  | expr '+' expr { $$ = expr_binary(EXPR_BINARY_PLUS, $1, $3); expr_free($1); expr_free($3); }
  | expr '*' expr { $$ = expr_binary(EXPR_BINARY_TIMES, $1, $3); expr_free($1); expr_free($3); }
  | expr '-' expr { $$ = expr_binary(EXPR_BINARY_MINUS, $1, $3); expr_free($1); expr_free($3); }
  | expr '/' expr { $$ = expr_binary(EXPR_BINARY_DIVIDE, $1, $3); expr_free($1); expr_free($3); }
  | expr '%' expr { $$ = expr_binary(EXPR_BINARY_MODULO, $1, $3); expr_free($1); expr_free($3); }
  | expr EXP expr { $$ = expr_binary(EXPR_BINARY_EXP, $1, $3); expr_free($1); expr_free($3); }
  | '-' expr %prec NEG  { $$ = expr_unary(EXPR_UNARY_NEG, $2); expr_free($2); }
  | '(' expr ')' { $$ = $2; }
  | '[' opt_exprlist ']' { $$ = expr_list($2); expr_list_free($2); }
  | '{' opt_dictlist '}' { $$ = expr_dict($2); expr_list_free($2); }
;

opt_exprlist:
    /* empty */  { $$ = NULL; }
  | exprlist     { $$ = $1; }
  | exprlist ',' { $$ = $1; }
;

exprlist:
    expr              { $$ = NULL; expr_list_append(&$$, $1); expr_free($1); }
  | exprlist ',' expr { expr_list_append(&$1, $3); $$ = $1; expr_free($3); }
;

opt_dictlist:
    /* empty */  { $$ = NULL; }
  | dictlist     { $$ = $1; }
  | dictlist ',' { $$ = $1; }
;

dictlist:
    expr ':' expr              { $$ = NULL; expr_list_append(&$$, $1); expr_list_append(&$$, $3); expr_free($1); expr_free($3); }
  | dictlist ',' expr ':' expr { expr_list_append(&$1, $3); expr_list_append(&$1, $5); $$ = $1; expr_free($3); expr_free($5); }
;

/* vim: set sw=4 et: */
