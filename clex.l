%option warn nodefault
%option yylineno
%x COMMENT

D        [0-9]
L        [a-zA-Z_]
H        [a-fA-F0-9]
FS       (f|F|l|L)
IS       (u|U|l|L)*

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "token.h"
#include "cgram.tab.h"

extern struct token *yytoken;
extern char *filename;

/* allocates and prepares token, saving pointer into yytoken */
void create_token(int category);
%}

%%

  /* eat whitespace */
[ \t\v\f\n]+            { }

  /* comments */
"//".*$                 { }
"/*"                    BEGIN(COMMENT);
<COMMENT>{
[^*]*                   /* eat comment in chunks */
"*"+[^*/]*              /* eat the lone star */
"*"+"/"                 BEGIN(INITIAL);
}

  /* keywords */
"..."                   { create_token(ELIPSIS); return ELIPSIS; }
"auto"                  { create_token(AUTO); return AUTO; }
"break"                 { create_token(BREAK); return BREAK; }
"case"                  { create_token(CASE); return CASE; }
"char"                  { create_token(CHAR); return CHAR; }
"const"                 { create_token(CONST); return CONST; }
"continue"              { create_token(CONTINUE); return CONTINUE; }
"default"               { create_token(DEFAULT); return DEFAULT; }
"do"                    { create_token(DO); return DO; }
"double"                { create_token(DOUBLE); return DOUBLE; }
"else"                  { create_token(ELSE); return ELSE; }
"enum"                  { create_token(ENUM); return ENUM; }
"extern"                { create_token(EXTERN); return EXTERN; }
"float"                 { create_token(FLOAT); return FLOAT; }
"for"                   { create_token(FOR); return FOR; }
"goto"                  { create_token(GOTO); return GOTO; }
"if"                    { create_token(IF); return IF; }
"int"                   { create_token(INT); return INT; }
"long"                  { create_token(LONG); return LONG; }
"register"              { create_token(REGISTER); return REGISTER; }
"return"                { create_token(RETURN); return RETURN; }
"short"                 { create_token(SHORT); return SHORT; }
"signed"                { create_token(SIGNED); return SIGNED; }
"sizeof"                { create_token(SIZEOF); return SIZEOF; }
"static"                { create_token(STATIC); return STATIC; }
"struct"                { create_token(STRUCT); return STRUCT; }
"switch"                { create_token(SWITCH); return SWITCH; }
"typedef"               { create_token(TYPEDEF); return TYPEDEF; }
"union"                 { create_token(UNION); return UNION; }
"unsigned"              { create_token(UNSIGNED); return UNSIGNED; }
"void"                  { create_token(VOID); return VOID; }
"volatile"              { create_token(VOLATILE); return VOLATILE; }
"while"                 { create_token(WHILE); return WHILE; }

  /* constants */
{D}+{IS}?               { create_token(ICON); return ICON; }

\'(\\.|[^"'"])+\'       { create_token(CCON); return CCON; }

{D}+{FS}?               { create_token(FCON); return FCON; }
{D}*"."{D}+{FS}?        { create_token(FCON); return FCON; }
{D}+"."{D}*{FS}?        { create_token(FCON); return FCON; }

L?\"(\\.|[^"\""])*\"    { create_token(STRING); return STRING; }

  /* operators */
">>="                   { create_token(SRASN); return SRASN; }
"<<="                   { create_token(SLASN); return SLASN; }
"+="                    { create_token(PLASN); return PLASN; }
"-="                    { create_token(MIASN); return MIASN; }
"*="                    { create_token(MUASN); return MUASN; }
"/="                    { create_token(DIASN); return DIASN; }
"%="                    { create_token(MOASN); return MOASN; }
"&="                    { create_token(ANASN); return ANASN; }
"^="                    { create_token(ERASN); return ERASN; }
"|="                    { create_token(ORASN); return ORASN; }
">>"                    { create_token(SHR); return SHR; }
"<<"                    { create_token(SHL); return SHL; }
"++"                    { create_token(INCOP); return INCOP; }
"--"                    { create_token(DECOP); return DECOP; }
"->"                    { create_token(FOLLOW); return FOLLOW; }
"&&"                    { create_token(ANDAND); return ANDAND; }
"||"                    { create_token(OROR); return OROR; }
"<="                    { create_token(LE); return LE; }
">="                    { create_token(GE); return GE; }
"=="                    { create_token(EQ); return EQ; }
"!="                    { create_token(NE); return NE; }
";"                     { create_token(SM); return SM; }
"{"                     { create_token(LC); return LC; }
"}"                     { create_token(RC); return RC; }
","                     { create_token(CM); return CM; }
":"                     { create_token(COLON); return COLON; }
"="                     { create_token(ASN); return ASN; }
"("                     { create_token(LP); return LP; }
")"                     { create_token(RP); return RP; }
"["                     { create_token(LB); return LB; }
"]"                     { create_token(RB); return RB; }
"."                     { create_token(DOT); return DOT; }
"&"                     { create_token(AND); return AND; }
"!"                     { create_token(BANG); return BANG; }
"~"                     { create_token(TILDE); return TILDE; }
"-"                     { create_token(MINUS); return MINUS; }
"+"                     { create_token(PLUS); return PLUS; }
"*"                     { create_token(MUL); return MUL; }
"/"                     { create_token(DIV); return DIV; }
"%"                     { create_token(MOD); return MOD; }
"<"                     { create_token(LT); return LT; }
">"                     { create_token(GT); return GT; }
"^"                     { create_token(ER); return ER; }
"|"                     { create_token(OR); return OR; }
"?"                     { create_token(QUEST); return QUEST; }

{L}({L}|{D})*           { create_token(IDENTIFIER); return IDENTIFIER; }
.                       { create_token(BAD_TOKEN); return BAD_TOKEN; }
<<EOF>>                 { return -1; }
%%

  /* TODO improve this */
int yywrap()
{
	return 1;
}

void create_token(int category)
{
	yytoken = malloc(sizeof(*yytoken));

	yytoken->category = category;
	yytoken->lineno = yylineno;

	yytoken->text = calloc(strlen(yytext)+1, sizeof(char));
	strcpy(yytoken->text, yytext);

	yytoken->filename = calloc(strlen(filename)+1, sizeof(char));
	strcpy(yytoken->filename, filename);

	if (yytoken->category == ICON)
		yytoken->ival = atoi(yytext);

	if (yytoken->category == STRING) {
		yytoken->sval = calloc(strlen(yytext)+1, sizeof(char));
		strcpy(yytoken->sval, yytext);
		/* TODO parse escaped characters */
	}

#ifdef DEBUG
	printf("line %d - category %d - lexeme: %s\n", yytoken->lineno, yytoken->category, yytoken->text);
#endif
}
