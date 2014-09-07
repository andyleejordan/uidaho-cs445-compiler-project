/* -*- mode: flex; -*- */
%option warn nodefault yylineno noyywrap nounput noinput
%x COMMENT STR CHR INC

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

#include "list.h"
#include "token.h"
#include "cgram.tab.h"

extern struct token *yytoken;
extern struct list *filenames;

bool cstdlib  = false;
bool cmath    = false;
bool ctime    = false;
bool cstring  = false;
bool fstream  = false;
bool iostream = false;
bool string   = false;
bool iomanip  = false;

/* allocates and prepares token, saving pointer into yytoken */
void new_token(int category)
{
	yytoken = token_create(category, yylineno, yytext, list_peek(filenames).filename);
}

void handle_include(char *s);
%}

%%

[ \t\v\f\n]+            { /* eat whitespace */ }

"//".*$                 { /* eat C++ comments */ }
"/*"                    { BEGIN(COMMENT); /* eat C comments */ }
<COMMENT>{
	[^*]*           { /* eat comment in chunks */ }
	"*"+[^*/]*      { /* eat the lone star */ }
	"*"+"/"         { BEGIN(INITIAL); }
}

  /* keywords */
"#include"              { BEGIN(INC); }
<INC>{
	[ \t]*          { /* eat whitespace */ }
	/* TODO add libraries to their own struct */
	"<cstdlib>"     { cstdlib  = true; BEGIN(INITIAL); }
	"<cmath>"       { cmath	   = true; BEGIN(INITIAL); }
	"<ctime>"       { ctime	   = true; BEGIN(INITIAL); }
	"<cstring>"     { cstring  = true; BEGIN(INITIAL); }
	"<fstream>"     { fstream  = true; BEGIN(INITIAL); }
	"<iostream>"    { iostream = true; BEGIN(INITIAL); }
	"<string>"      { string   = true; BEGIN(INITIAL); }
	"<iomanip>"     { iomanip  = true; BEGIN(INITIAL); }
	"<"[^<>]*">"    { BEGIN(INITIAL); /* TODO error on unrecognized libraries */ }
	\"[^\"]+\"      { handle_include(yytext); BEGIN(INITIAL); }
	.               { new_token(BADTOKEN); return BADTOKEN; }
}

"auto"                  { new_token(AUTO); return AUTO; }
"bool"                  { new_token(BOOL); return BOOL; }
"break"                 { new_token(BREAK); return BREAK; }
"case"                  { new_token(CASE); return CASE; }
"char"                  { new_token(CHAR); return CHAR; }
"class"                 { new_token(CLASS); return CLASS; }
"const"                 { new_token(CONST); return CONST; }
"continue"              { new_token(CONTINUE); return CONTINUE; }
"default"               { new_token(DEFAULT); return DEFAULT; }
"delete"                { new_token(DELETE); return DELETE; }
"do"                    { new_token(DO); return DO; }
"double"                { new_token(DOUBLE); return DOUBLE; }
"..."                   { new_token(ELIPSIS); return ELIPSIS; }
"else"                  { new_token(ELSE); return ELSE; }
"enum"                  { new_token(ENUM); return ENUM; }
"extern"                { new_token(EXTERN); return EXTERN; }
"false"                 { new_token(FALSE); return FALSE; }
"float"                 { new_token(FLOAT); return FLOAT; }
"for"                   { new_token(FOR); return FOR; }
"friend"                { new_token(FRIEND); return FRIEND; }
"goto"                  { new_token(GOTO); return GOTO; }
"if"                    { new_token(IF); return IF; }
"int"                   { new_token(INT); return INT; }
"long"                  { new_token(LONG); return LONG; }
"namespace"             { new_token(NAMESPACE); return NAMESPACE; }
"new"                   { new_token(NEW); return NEW; }
"private"               { new_token(PRIVATE); return PRIVATE; }
"protected"             { new_token(PROTECTED); return PROTECTED; }
"public"                { new_token(PUBLIC); return PUBLIC; }
"register"              { new_token(REGISTER); return REGISTER; }
"return"                { new_token(RETURN); return RETURN; }
"short"                 { new_token(SHORT); return SHORT; }
"signed"                { new_token(SIGNED); return SIGNED; }
"sizeof"                { new_token(SIZEOF); return SIZEOF; }
"static"                { new_token(STATIC); return STATIC; }
"struct"                { new_token(STRUCT); return STRUCT; }
"switch"                { new_token(SWITCH); return SWITCH; }
"this"                  { new_token(THIS); return THIS; }
"true"                  { new_token(TRUE); return TRUE; }
"typedef"               { new_token(TYPEDEF); return TYPEDEF; }
"union"                 { new_token(UNION); return UNION; }
"unsigned"              { new_token(UNSIGNED); return UNSIGNED; }
"using"                 { new_token(USING); return USING; }
"virtual"               { new_token(VIRTUAL); return VIRTUAL; }
"void"                  { new_token(VOID); return VOID; }
"volatile"              { new_token(VOLATILE); return VOLATILE; }
"while"                 { new_token(WHILE); return WHILE; }

  /* constants */
{D}+{IS}?               { new_token(ICON); return ICON; }
{D}+{FS}?               { new_token(FCON); return FCON; }
{D}*"."{D}+{FS}?        { new_token(FCON); return FCON; }
{D}+"."{D}*{FS}?        { new_token(FCON); return FCON; }

\'                      { new_token(CCON); BEGIN(CHR); }
<CHR>{
	\'              { token_append_text(yytoken, yytext); BEGIN(INITIAL); return CCON; }
	\n              { new_token(BADTOKEN); return BADTOKEN; }
	"\\'"           { yytoken->ival = '\''; token_append_text(yytoken, yytext); }
	"\\\""          { yytoken->ival = '"';  token_append_text(yytoken, yytext); }
	"\\?"           { yytoken->ival = '\?'; token_append_text(yytoken, yytext); }
	"\\a"           { yytoken->ival = '\a'; token_append_text(yytoken, yytext); }
	"\\b"           { yytoken->ival = '\b'; token_append_text(yytoken, yytext); }
	"\\f"           { yytoken->ival = '\f'; token_append_text(yytoken, yytext); }
	"\\n"           { yytoken->ival = '\n'; token_append_text(yytoken, yytext); }
	"\\r"           { yytoken->ival = '\r'; token_append_text(yytoken, yytext); }
	"\\t"           { yytoken->ival = '\t'; token_append_text(yytoken, yytext); }
	"\\v"           { yytoken->ival = '\v'; token_append_text(yytoken, yytext); }
	"\\\\"          { yytoken->ival = '\\'; token_append_text(yytoken, yytext); }
	[^\\"'"]        { yytoken->ival = *yytext; token_append_text(yytoken, yytext); }
	.               { new_token(BADTOKEN); return BADTOKEN; }
}

L?\"                    { new_token(SCON); BEGIN(STR); }
<STR>{
	\"              { token_append_sval_char(yytoken, '\0'); token_append_text(yytoken, yytext); BEGIN(INITIAL); return SCON; }
	\n              { new_token(BADTOKEN); return BADTOKEN; }
	"\\'"           { token_append_sval_char(yytoken, '\''); token_append_text(yytoken, yytext); }
	"\\\""          { token_append_sval_char(yytoken, '"');  token_append_text(yytoken, yytext); }
	"\\?"           { token_append_sval_char(yytoken, '\?'); token_append_text(yytoken, yytext); }
	"\\a"           { token_append_sval_char(yytoken, '\a'); token_append_text(yytoken, yytext); }
	"\\b"           { token_append_sval_char(yytoken, '\b'); token_append_text(yytoken, yytext); }
	"\\f"           { token_append_sval_char(yytoken, '\f'); token_append_text(yytoken, yytext); }
	"\\n"           { token_append_sval_char(yytoken, '\n'); token_append_text(yytoken, yytext); }
	"\\r"           { token_append_sval_char(yytoken, '\r'); token_append_text(yytoken, yytext); }
	"\\t"           { token_append_sval_char(yytoken, '\t'); token_append_text(yytoken, yytext); }
	"\\v"           { token_append_sval_char(yytoken, '\v'); token_append_text(yytoken, yytext); }
	"\\\\"          { token_append_sval_char(yytoken, '\\'); token_append_text(yytoken, yytext); }
	[^\\\n\"]+      { token_append_sval_string(yytoken, yytext); token_append_text(yytoken, yytext); }
	.               { new_token(BADTOKEN); return BADTOKEN; }
}

  /* operators */
">>="                   { new_token(SRASN); return SRASN; }
"<<="                   { new_token(SLASN); return SLASN; }
"+="                    { new_token(PLASN); return PLASN; }
"-="                    { new_token(MIASN); return MIASN; }
"*="                    { new_token(MUASN); return MUASN; }
"/="                    { new_token(DIASN); return DIASN; }
"%="                    { new_token(MOASN); return MOASN; }
"&="                    { new_token(ANASN); return ANASN; }
"^="                    { new_token(ERASN); return ERASN; }
"|="                    { new_token(ORASN); return ORASN; }
">>"                    { new_token(SHR); return SHR; }
"<<"                    { new_token(SHL); return SHL; }
"++"                    { new_token(INCOP); return INCOP; }
"--"                    { new_token(DECOP); return DECOP; }
"->"                    { new_token(FOLLOW); return FOLLOW; }
"&&"                    { new_token(ANDAND); return ANDAND; }
"||"                    { new_token(OROR); return OROR; }
"<="                    { new_token(LE); return LE; }
">="                    { new_token(GE); return GE; }
"=="                    { new_token(EQ); return EQ; }
"!="                    { new_token(NE); return NE; }
";"                     { new_token(SM); return SM; }
"{"                     { new_token(LC); return LC; }
"}"                     { new_token(RC); return RC; }
","                     { new_token(CM); return CM; }
":"                     { new_token(COLON); return COLON; }
"="                     { new_token(ASN); return ASN; }
"("                     { new_token(LP); return LP; }
")"                     { new_token(RP); return RP; }
"["                     { new_token(LB); return LB; }
"]"                     { new_token(RB); return RB; }
"."                     { new_token(DOT); return DOT; }
"&"                     { new_token(AND); return AND; }
"!"                     { new_token(BANG); return BANG; }
"~"                     { new_token(TILDE); return TILDE; }
"-"                     { new_token(MINUS); return MINUS; }
"+"                     { new_token(PLUS); return PLUS; }
"*"                     { new_token(MUL); return MUL; }
"/"                     { new_token(DIV); return DIV; }
"%"                     { new_token(MOD); return MOD; }
"<"                     { new_token(LT); return LT; }
">"                     { new_token(GT); return GT; }
"^"                     { new_token(ER); return ER; }
"|"                     { new_token(OR); return OR; }
"?"                     { new_token(QUEST); return QUEST; }

{L}({L}|{D})*           { new_token(IDENTIFIER); return IDENTIFIER; }

.                       { new_token(BADTOKEN); return BADTOKEN; }

<<EOF>>                 { yypop_buffer_state(); list_pop(filenames); yylineno = 1; if (!YY_CURRENT_BUFFER) yyterminate(); }

%%

void handle_include(char *s)
{
	size_t len = strlen(s) - 2; /* size without surrounding quotes */
	char *filename = calloc(len+1, sizeof(char));
	if (filename == NULL)
		goto error_malloc;
	strncpy(filename, s+1, len); /* copy substring */
	filename[len] = '\0';
	list_push(filenames, (union data)filename);

	yyin = fopen(filename, "r");
	if (yyin == NULL)
		goto error_fopen;
	yylineno = 1;
	yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

	return;

 error_malloc: {
		perror("handle_include()");
		exit(EXIT_FAILURE);
	}

 error_fopen: {
		perror("handle_include: fopen(filename)");
		exit(EXIT_FAILURE);
	}
}
