%{
// RT3lex.l - Lexer for RT3 scene files

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma warning ( disable: 4003 4018 4996 )
struct VectorType;
#include "RT3parse.hpp"
extern YYSTYPE yylval;
%}
%option noyywrap
delim         [ \t]
whitesp       {delim}+
digit         [0-9]
number        [-]?{digit}*[.]?{digit}+

%%
{number}  { sscanf_s(yytext, "%lf", &yylval); return NUMBER;}
"+"       { return PLUS; }
"-"       { return MINUS; }
"*"       { return ASTERISK; }
"("       { return LPAREN; }
")"       { return RPAREN; }
"\n"      { return NEWLINE; }
"<"       { return LANG; }
">"       { return RANG; }
","       { return COMMA; }
{whitesp} { /* No action and no return */}
