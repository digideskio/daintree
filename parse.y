%{
    #include <program.h>
%}

%define api.value.type {union token}

%parse-param {Program *program}
%error-verbose

%token END_OF_FILE 0 "$end"
%token NL

%type <stmt> stmt line

%right EQUALS

%nonassoc <identifier> IDENTIFIER
%nonassoc <number> NUMBER

%%

input:
    /* empty */
  | input line { if ($2) { program->stmt = $2; } }
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
    IDENTIFIER EQUALS NUMBER { $$ = stmt_new($1, $3); free($1); }
;

/* vim: set sw=4 et: */
