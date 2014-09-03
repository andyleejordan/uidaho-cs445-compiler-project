%option warn nodefault
%option yylineno

/* macros borrowed from Jutta Degener
   http://www.lysator.liu.se/c/ANSI-C-grammar-l.html */
D        [0-9]
L        [a-zA-Z_]
H        [a-fA-F0-9]
FS       (f|F|l|L)
IS       (u|U|l|L)*
QUOTE    \"
TICK     \'

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "cgram.tab.h"

extern int yylineno;
extern int yychar;

void comment();
%}

%%

  /* eat whitespace */
[ \t\v\f\n]+            { }

  /* comments */
"/*"                    { comment(); }

  /* keywords */
"..."                   { return ELIPSIS; }
"auto"                  { return AUTO; }
"break"                 { return BREAK; }
"case"                  { return CASE; }
"char"                  { return CHAR; }
"const"                 { return CONST; }
"continue"              { return CONTINUE; }
"default"               { return DEFAULT; }
"do"                    { return DO; }
"double"                { return DOUBLE; }
"else"                  { return ELSE; }
"enum"                  { return ENUM; }
"extern"                { return EXTERN; }
"float"                 { return FLOAT; }
"for"                   { return FOR; }
"goto"                  { return GOTO; }
"if"                    { return IF; }
"int"                   { return INT; }
"long"                  { return LONG; }
"register"              { return REGISTER; }
"return"                { return RETURN; }
"short"                 { return SHORT; }
"signed"                { return SIGNED; }
"sizeof"                { return SIZEOF; }
"static"                { return STATIC; }
"struct"                { return STRUCT; }
"switch"                { return SWITCH; }
"typedef"               { return TYPEDEF; }
"union"                 { return UNION; }
"unsigned"              { return UNSIGNED; }
"void"                  { return VOID; }
"volatile"              { return VOLATILE; }
"while"                 { return WHILE; }

  /* constants */
{D}+{IS}?               { return ICON; }

"'"(\\.|[^\{TICK}])+"'" { return CCON; }

{D}+{FS}?               { return FCON; }
{D}*"."{D}+{FS}?        { return FCON; }
{D}+"."{D}*{FS}?        { return FCON; }

L?{QUOTE}(\\.|[^\{QUOTE}])*{QUOTE}  { return STRING; }

  /* operators */
">>="                   { return SRASN; }
"<<="                   { return SLASN; }
"+="                    { return PLASN; }
"-="                    { return MIASN; }
"*="                    { return MUASN; }
"/="                    { return DIASN; }
"%="                    { return MOASN; }
"&="                    { return ANASN; }
"^="                    { return ERASN; }
"|="                    { return ORASN; }
">>"                    { return SHR; }
"<<"                    { return SHL; }
"++"                    { return INCOP; }
"--"                    { return DECOP; }
"->"                    { return FOLLOW; }
"&&"                    { return ANDAND; }
"||"                    { return OROR; }
"<="                    { return LE; }
">="                    { return GE; }
"=="                    { return EQ; }
"!="                    { return NE; }
";"                     { return SM; }
"{"                     { return LC; }
"}"                     { return RC; }
","                     { return CM; }
":"                     { return COLON; }
"="                     { return ASN; }
"("                     { return LP; }
")"                     { return RP; }
"["                     { return LB; }
"]"                     { return RB; }
"."                     { return DOT; }
"&"                     { return AND; }
"!"                     { return BANG; }
"~"                     { return TILDE; }
"-"                     { return MINUS; }
"+"                     { return PLUS; }
"*"                     { return MUL; }
"/"                     { return DIV; }
"%"                     { return MOD; }
"<"                     { return LT; }
">"                     { return GT; }
"^"                     { return ER; }
"|"                     { return OR; }
"?"                     { return QUEST; }

{L}({L}|{D})*           { return IDENTIFIER; }
.                       { return BAD_TOKEN; }
%%

int yywrap()
{
  return 1;
}

void comment()
{
  /* comment consumption code also from Jutta Degener */
  char c, c1;

 loop:
  while ((c = input()) != '*' && c != 0)
    {
      putchar(c);
    }
  if ((c1 = input()) != '/' && c != 0)
    {
      unput(c1);
      goto loop;
    }
  if (c != 0)
    {
      putchar(c1);
    }
}
