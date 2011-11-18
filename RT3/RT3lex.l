%{
// RT3lex.l - Lexer for RT3 scene files

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Parser.h"

#pragma warning ( disable: 4003 4018 4996 )
#include "RT3parse.hpp"

extern YYSTYPE yylval;
extern char buf[256]; 
map<string, string> definitions;
string currentIdentifier;
%}
%option noyywrap
delim         [ \t]
whitesp       {delim}+
digit         [0-9]
integer       [+-]?{digit}+
real          [+-]?{digit}*"."{digit}+
identifier    [a-zA-Z_][a-zA-Z0-9_]*
string        \"[^"]*
comment1      "/*"[^/]*"*/"
comment2      "//".*
definition    "{"[^#]*"}#"
%x DEFINE
%%
{integer} { sscanf_s(yytext, "%d", &yylval); return NUMBER_INTEGER;}
{real}	  { sscanf_s(yytext, "%f", &yylval); return NUMBER_FLOAT;}
"+"       { return PLUS; }
"-"       { return MINUS; }
"*"       { return ASTERISK; }
"/"       { return SLASH; }
"("       { return LPAREN; }
")"       { return RPAREN; }
"{"       { return LCURL; }
"}"       { return RCURL; }
"<"       { return LANG; }
">"       { return RANG; }
"["       { return LSQ; }
"]"       { return RSQ; }
","       { return COMMA; }
"="       { return EQ; }
"#"       { BEGIN(DEFINE); }
"\n"      { /* return NEWLINE; */ line_no++; }
{whitesp}  { /* No action and no return */}
{string}   { 
				if(yytext[yyleng-1] == '\\')
					yymore();
				else 
				{
					yyinput();
					yylval.str = new string(yytext+1);
				}
				return STRING;
		 }
{comment1} { for(int i = 0; i < yyleng; i++) if(yytext[i] == '\n') line_no++; }
{comment2} { for(int i = 0; i < yyleng; i++) if(yytext[i] == '\n') line_no++; }

Film				{ return FILM; }
width				{ return FILM_WIDTH; }
height				{ return FILM_HEIGHT; }
samples				{ return FILM_SAMPLES; }
background			{ return FILM_BACKGROUND; }

Perspective			{ return PERSPECTIVE_CAMERA; }
Orthographic		{ return ORTHOGRAPHIC_CAMERA; }
origin				{ return CAMERA_ORIGIN; }
up					{ return CAMERA_UP; }
right				{ return CAMERA_RIGHT; }
fov					{ return CAMERA_ANGLE; }
look_at				{ return CAMERA_LOOKAT; }

ka					{ return MATERIAL_KA; }
kd					{ return MATERIAL_KD; }
reflect				{ return MATERIAL_REFLECTED_COLOR; }
transmit			{ return MATERIAL_TRANSMITTED_COLOR; }
ior					{ return MATERIAL_IOR; }

Image				{ return TEXTURE_IMAGE; }
Checker				{ return TEXTURE_CHECKER; }

Sphere				{ return SPHERE; }
Plane				{ return PLANE; }
Box					{ return BOX; }
Disc                { return DISC; }
Cylinder            { return CYLINDER; }
Triangle			{ return TRIANGLE; }
Mesh				{ return MESH; }
KDTree				{ return KDTREE; }
leafsize			{ return KDTREE_LEAFSIZE; }
Group				{ return GROUP; }
Transform           { return TRANSFORMED_OBJECT; }

translate			{ return TRANSLATE; }
rotate				{ return ROTATE; }
scale				{ return SCALE; }

PointLight			{ return POINTLIGHT; }
TriangleLight		{ return TRIANGLELIGHT; }
Integer				{ return INTEGER_TYPE; }
Float				{ return FLOAT_TYPE; }
Vector				{ return VECTOR_TYPE; }
Spectrum			{ return SPECTRUM_TYPE; }
Material			{ return MATERIAL_TYPE; }
Object				{ return OBJECT_TYPE; }
Light				{ return LIGHT_TYPE; }
Camera				{ return CAMERA_TYPE; }

x                   { return XVECTOR; }
y					{ return YVECTOR; }
z					{ return ZVECTOR; }

<INITIAL>{identifier} { 
						map<string,string>::iterator it = definitions.find(yytext);
						if(it != definitions.end())
						{
							string s = it->second;
							for(int i = s.length()-1; i >= 0; i--) unput(s[i]);
						}
						else
						{
							yylval.str = new string(yytext);
							return IDENTIFIER;
						}
				    }
<DEFINE>{identifier} {
						currentIdentifier = yytext;
					}
<DEFINE>{definition} {
						char *p = yytext;
						while(*p) {
							if(*p == '\n') line_no++;
							p++;
						}
						string s = yytext+1;
						s = s.substr(0,s.length()-2);
						definitions[currentIdentifier] = s;
						BEGIN(INITIAL);
					}
.					{
						sprintf_s(buf, 256, "Unrecognized string: %s", yytext);
						yyerror(buf);
					}
%%
