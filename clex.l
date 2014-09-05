/* -*- mode: flex; -*- */
%option warn nodefault yylineno noyywrap
%x COMMENT
%x STR
%x CHR
%x INC

D        [0-9]
L        [a-zA-Z_]
H        [a-fA-F0-9]
FS       (f|F|l|L)
IS       (u|U|l|L)*

%{
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "token.h"
#include "cgram.tab.h"

extern struct token *yytoken;
extern char *filename;

bool cstdlib  = false;
bool cmath    = false;
bool ctime    = false;
bool cstring  = false;
bool fstream  = false;
bool iostream = false;
bool string   = false;
bool iomanip  = false;

/* allocates and prepares token, saving pointer into yytoken */
void create_token(int category);

void yytoken_append_char_sval(char c);
void yytoken_append_string_sval(char *s);
void yytoken_append_text(char *s);
%}

%%

[ \t\v\f\n]+            { /* eat whitespace */ }

"//".*$                 { /* eat C++ comments */ }
"/*"                    { BEGIN(COMMENT); /* eat C comments */ }
<COMMENT>{
	[^*]*           { /* eat comment in chunks */ }
	"*"+[^*/]*      { /* eat the lone star */ }
	"*"+"/"         { BEGIN(INITIAL); /* stop eating comment */ }
}

  /* keywords */
"#include"              { BEGIN(INC); }
<INC>{
	[ \t\v\f]+      { /* eat whitespace */ }
	"<cstdlib>"     { cstdlib  = true; BEGIN(INITIAL); }
	"<cmath>"       { cmath	   = true; BEGIN(INITIAL); }
	"<ctime>"       { ctime	   = true; BEGIN(INITIAL); }
	"<cstring>"     { cstring  = true; BEGIN(INITIAL); }
	"<fstream>"     { fstream  = true; BEGIN(INITIAL); }
	"<iostream>"    { iostream = true; BEGIN(INITIAL); }
	"<string>"      { string   = true; BEGIN(INITIAL); }
	"<iomanip>"     { iomanip  = true; BEGIN(INITIAL); }
	"<"[^<>]+">"    { BEGIN(INITIAL); /* TODO error on unrecognized libraries */ }
}
"auto"                  { create_token(AUTO); return AUTO; }
"bool"                  { create_token(BOOL); return BOOL; }
"break"                 { create_token(BREAK); return BREAK; }
"case"                  { create_token(CASE); return CASE; }
"char"                  { create_token(CHAR); return CHAR; }
"class"                 { create_token(CLASS); return CLASS; }
"const"                 { create_token(CONST); return CONST; }
"continue"              { create_token(CONTINUE); return CONTINUE; }
"default"               { create_token(DEFAULT); return DEFAULT; }
"delete"                { create_token(DELETE); return DELETE; }
"do"                    { create_token(DO); return DO; }
"double"                { create_token(DOUBLE); return DOUBLE; }
"..."                   { create_token(ELIPSIS); return ELIPSIS; }
"else"                  { create_token(ELSE); return ELSE; }
"enum"                  { create_token(ENUM); return ENUM; }
"extern"                { create_token(EXTERN); return EXTERN; }
"false"                 { create_token(FALSE); return FALSE; }
"float"                 { create_token(FLOAT); return FLOAT; }
"for"                   { create_token(FOR); return FOR; }
"friend"                { create_token(FRIEND); return FRIEND; }
"goto"                  { create_token(GOTO); return GOTO; }
"if"                    { create_token(IF); return IF; }
"int"                   { create_token(INT); return INT; }
"long"                  { create_token(LONG); return LONG; }
"namespace"             { create_token(NAMESPACE); return NAMESPACE; }
"new"                   { create_token(NEW); return NEW; }
"private"               { create_token(PRIVATE); return PRIVATE; }
"protected"             { create_token(PROTECTED); return PROTECTED; }
"public"                { create_token(PUBLIC); return PUBLIC; }
"register"              { create_token(REGISTER); return REGISTER; }
"return"                { create_token(RETURN); return RETURN; }
"short"                 { create_token(SHORT); return SHORT; }
"signed"                { create_token(SIGNED); return SIGNED; }
"sizeof"                { create_token(SIZEOF); return SIZEOF; }
"static"                { create_token(STATIC); return STATIC; }
"struct"                { create_token(STRUCT); return STRUCT; }
"switch"                { create_token(SWITCH); return SWITCH; }
"this"                  { create_token(THIS); return THIS; }
"true"                  { create_token(TRUE); return TRUE; }
"typedef"               { create_token(TYPEDEF); return TYPEDEF; }
"union"                 { create_token(UNION); return UNION; }
"unsigned"              { create_token(UNSIGNED); return UNSIGNED; }
"using"                 { create_token(USING); return USING; }
"virtual"               { create_token(VIRTUAL); return VIRTUAL; }
"void"                  { create_token(VOID); return VOID; }
"volatile"              { create_token(VOLATILE); return VOLATILE; }
"while"                 { create_token(WHILE); return WHILE; }

  /* constants */
{D}+{IS}?               { create_token(ICON); return ICON; }
{D}+{FS}?               { create_token(FCON); return FCON; }
{D}*"."{D}+{FS}?        { create_token(FCON); return FCON; }
{D}+"."{D}*{FS}?        { create_token(FCON); return FCON; }

\'                      { create_token(CCON); BEGIN(CHR); }
<CHR>{
	\'              { BEGIN(INITIAL); yytoken_append_text(yytext); return CCON; }
	\n              { BEGIN(INITIAL); return -2; } /* unterminated string constant */
	"\\'"           { yytoken->ival = '\''; yytoken_append_text(yytext); }
	"\\\""          { yytoken->ival = '"'; yytoken_append_text(yytext); }
	"\\?"           { yytoken->ival = '\?'; yytoken_append_text(yytext); }
	"\\\\"          { yytoken->ival = '\\'; yytoken_append_text(yytext); }
	"\\a"           { yytoken->ival = '\a'; yytoken_append_text(yytext); }
	"\\b"           { yytoken->ival = '\b'; yytoken_append_text(yytext); }
	"\\f"           { yytoken->ival = '\f'; yytoken_append_text(yytext); }
	"\\n"           { yytoken->ival = '\n'; yytoken_append_text(yytext); }
	"\\r"           { yytoken->ival = '\r'; yytoken_append_text(yytext); }
	"\\t"           { yytoken->ival = '\t'; yytoken_append_text(yytext); }
	"\\v"           { yytoken->ival = '\v'; yytoken_append_text(yytext); }
	[^\\"'"]        { yytoken->ival = *yytext; yytoken_append_text(yytext); }
}

L?\"                    { create_token(SCON); BEGIN(STR); }
<STR>{
	\"              { yytoken_append_char_sval('\0'); yytoken_append_text(yytext); BEGIN(INITIAL); return SCON; /* end of string*/ }
	\n              { BEGIN(INITIAL); return -2; } /* unterminated string constant */
	"\\'"           { yytoken_append_char_sval('\''); yytoken_append_text(yytext); }
	"\\\""          { yytoken_append_char_sval('"'); yytoken_append_text(yytext); }
	"\\?"           { yytoken_append_char_sval('\?'); yytoken_append_text(yytext); }
	"\\"            { yytoken_append_char_sval('\\'); yytoken_append_text(yytext); }
	"\\a"           { yytoken_append_char_sval('\a'); yytoken_append_text(yytext); }
	"\\b"           { yytoken_append_char_sval('\b'); yytoken_append_text(yytext); }
	"\\f"           { yytoken_append_char_sval('\f'); yytoken_append_text(yytext); }
	"\\n"           { yytoken_append_char_sval('\n'); yytoken_append_text(yytext); }
	"\\r"           { yytoken_append_char_sval('\r'); yytoken_append_text(yytext); }
	"\\t"           { yytoken_append_char_sval('\t'); yytoken_append_text(yytext); }
	"\\v"           { yytoken_append_char_sval('\v'); yytoken_append_text(yytext); }
	[^\\\n\"]+      { yytoken_append_string_sval(yytext); yytoken_append_text(yytext); }
}

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

<<EOF>>                 { return 0; }
%%

void create_token(int category)
{
	yytoken = malloc(sizeof(*yytoken));

	yytoken->category = category;
	yytoken->lineno = yylineno;

	yytoken->text = calloc(strlen(yytext)+1, sizeof(char));
	strcpy(yytoken->text, yytext);

	yytoken->filename = calloc(strlen(filename)+1, sizeof(char));
	strcpy(yytoken->filename, filename);

	if (category == ICON)
		yytoken->ival = atoi(yytext);

	if (category == SCON)
		yytoken->sval = calloc(128, sizeof(char));

#ifdef DEBUG
	printf("line %d - category %d - lexeme: %s\n", yytoken->lineno, yytoken->category, yytoken->text);
#endif
}

/* reallocate yytoken->sval to append string and terminating null */
void yytoken_realloc_sval(char *s)
{
	size_t size = sizeof(yytoken->sval)+sizeof(s)+sizeof(char);
	yytoken->sval = realloc(yytoken->sval, size);
}

/* append char literal to yytoken->sval string (to parse escapes) */
void yytoken_append_char_sval(char c)
{
	size_t len = strlen(yytoken->sval);
	yytoken_realloc_sval(&c);
	yytoken->sval[len] = c;
}

/* append string literal to yytoken->sval string (with null) */
void yytoken_append_string_sval(char *s)
{
	yytoken_realloc_sval(s);
	strcat(yytoken->sval, s);
}

/* reallocate yytoken->text to append string and terminating null */
void yytoken_realloc_text(char *s)
{
	size_t size = sizeof(yytoken->text)+sizeof(s)+sizeof(char);
	yytoken->text = realloc(yytoken->text, size);
}

/* append string literal yytoken->text (with null) */
void yytoken_append_text(char *s)
{
	yytoken_realloc_text(s);
	strcat(yytoken->text, s);
}
