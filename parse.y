%{
    #include <program.h>
%}

%define api.value.type {union token}

%parse-param {Program *program}
%error-verbose

%token END_OF_FILE 0 "$end"
%token NL

%type <stmt> stmt

%nonassoc <identifier> IDENTIFIER

%%

input:
    /* empty */
  | input line
;

line_separator:
    NL
  | ';'
;

line:
    line_separator
  | stmt line_separator
  | stmt END_OF_FILE
;

stmt:
    IDENTIFIER { $$ = $1; }
;

/* vim: set sw=4 et: */
