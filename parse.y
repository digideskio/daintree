%{
    #include <ast.h>
%}

%define api.value.type {union token}

%parse-param {Program *program}
%error-verbose

%token END_OF_FILE 0 "$end"
%token NL PRINT

%type <stmt> stmt line
%type <expr> expr

%right EQUALS

%nonassoc <identifier> IDENTIFIER
%nonassoc <number> NUMBER

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
    IDENTIFIER EQUALS expr { $$ = stmt_assign($1, $3); free($1); expr_free($3); }
  | PRINT expr { $$ = stmt_print($2); expr_free($2); }
;

expr:
    NUMBER { $$ = expr_number($1); }
  | IDENTIFIER { $$ = expr_identifier($1); free($1); }
;

/* vim: set sw=4 et: */
