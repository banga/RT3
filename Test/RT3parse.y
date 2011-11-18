/* RT3.y - parser for RT3 scene files */
%{
#include <malloc.h>
#include <stdlib.h>
struct VectorType {
	double x, y, z;
};
extern int yylex( void );
extern int yyerror( char* );
%}
/* BISON Declarations */
%union {
	double num;
	struct VectorType* val;
}
%token NEWLINE PLUS MINUS ASTERISK LPAREN RPAREN LANG RANG COMMA
%token <num> NUMBER
%type <num> number
%type <val> vector expr term factor
/* Grammar follows */
%%
input:              /* empty string */
    | input line
    ;
line: NEWLINE
    | expr NEWLINE           { printf("\t<%.10g,%.10g,%.10g>\n",$1->x,$1->y,$1->z); }
    ;
expr: expr PLUS term    
	{
		$$->x = $1->x + $3->x;
		$$->y = $1->y + $3->y;
		$$->z = $1->z + $3->z;
	}
    | expr MINUS term
	{
		$$->x = $1->x - $3->x;
		$$->y = $1->y - $3->y;
		$$->z = $1->z - $3->z;
	}
    | term
    ;
term: term ASTERISK factor
	{
		$$->x = $1->x * $3->x;
		$$->y = $1->y * $3->y;
		$$->z = $1->z * $3->z;
	}
    | factor
    ;
factor:  LPAREN expr RPAREN  { $$ = $2 }
      | vector
      ;
vector: LANG number COMMA number COMMA number RANG
	{
		$$ = (struct VectorType*)malloc(sizeof(struct VectorType));
		$$->x = $2;
		$$->y = $4;
		$$->z = $6;
	}
	;
number: NUMBER { $$ = $1 }
%%
/*--------------------------------------------------------*/
/* Additional C code */
/* Error processor for yyparse */
#include <stdio.h>
int yyerror(char *s)        /* called by yyparse on error */
{
    printf("%s\n",s);
    return(0);
}
