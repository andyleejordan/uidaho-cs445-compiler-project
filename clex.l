/*
 * clex.l - Flex scanner for 120++.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

%option warn nounput noinput
%option header-file="clex.h"
%option yylineno noyywrap
%x COMMENT STR CHR CHREND INC

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
#include <libgen.h>

#include "list.h"
#include "tree.h"
#include "token.h"
#include "cgram.tab.h"

struct typename {
	char *name;
	int category;
};

extern struct list *typenames;
extern struct list *filenames;
extern char *error_buf;
extern void handle_error(char *c);
extern char *current_filename();

struct token *yytoken = NULL;

bool usingstd = false;
bool cstdlib  = false;
bool cmath    = false;
bool ctime    = false;
bool cstring  = false;
bool fstream  = false;
bool iostream = false;
bool string   = false;
bool iomanip  = false;

/* print error message to stderr and terminate (for now) */
void yyerror(const char *s)
{
	fprintf (stderr, "File %s line %d, token %s: %s\n",
	         (const char *)list_peek(filenames), yylineno, yytext, s);
	exit(1);
}

/*
 * creates a token with the necessary information, then allocates a
 * tree node as a leaf for the token, saving it into yylval for Bison
 */
void new_token(int category)
{
	yytoken = token_create(category, yylineno, yytext,
                                       (const char *)list_peek(filenames));
	yylval.t = tree_init(NULL, yytoken);
}

bool typename_compare(const char *s, const struct typename *t)
{
	return 0 == strcmp(s, t->name);
}

/*
 * inserts a new typename into typenames list
 */
void typenames_insert(char *s, int category)
{
	struct list_node *l = list_find(typenames, (void *)s,
					(bool (*)(void *, void *))&typename_compare);
	if (!list_end(l)) {
		sprintf(error_buf, "Identifier %s already in typenames\n", s);
		handle_error(error_buf);
	}
	struct typename *type = malloc(sizeof(*type));
	type->name = strdup(s);
	type->category = category;
	list_push(typenames, type);
}

/*
 * unwraps a tree leaf and passes token text as name to insert function
 */
void typenames_insert_tree(struct tree *t, int category)
{
	typenames_insert(((struct token *)t->data)->text, category);
}

/*
 * returns corresponding category for identifier
 */
int check_identifier(const char *s)
{
	struct list_node *l = list_find(typenames, (void *)s,
					(bool (*)(void *, void *))&typename_compare);
	if (list_end(l)) {
		new_token(IDENTIFIER);
		return IDENTIFIER;
	}
	else {
		return ((struct typename *)l->data)->category;
	}
}

void handle_include(const char *s);
%}

%%

[ \r\t\v\f\n]*          { /* eat whitespace */ }

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
	"<cstdlib>"     { cstdlib  = true; BEGIN(INITIAL); }
	"<cmath>"       { cmath	   = true; BEGIN(INITIAL); }
	"<ctime>"       { ctime	   = true; BEGIN(INITIAL); }
	"<cstring>"     { cstring  = true; BEGIN(INITIAL); }
	"<fstream>"     { fstream  = true; BEGIN(INITIAL); }
	"<iostream>"    { iostream = true; BEGIN(INITIAL); }
	"<string>"      { string   = true; typenames_insert("string", CLASS_NAME); BEGIN(INITIAL); }
	"<iomanip>"     { iomanip  = true; BEGIN(INITIAL); }
	\"[^\"]+\"      { handle_include(yytext); BEGIN(INITIAL); }
	"<"[^<>]*">"    { yyerror("Unrecognized system library"); }
	<<EOF>>         { yyerror("Unexpected EOF"); }
	.               { yyerror("Unrecognized token following #include"); }
}
"using namespace std;"  { usingstd = true; }
"bool"                  { new_token(BOOL); return BOOL; }
"break"                 { new_token(BREAK); return BREAK; }
"case"                  { new_token(CASE); return CASE; }
"char"                  { new_token(CHAR); return CHAR; }
"class"                 { new_token(CLASS); return CLASS; }
"continue"              { new_token(CONTINUE); return CONTINUE; }
"default"               { new_token(DEFAULT); return DEFAULT; }
"delete"                { new_token(DELETE); return DELETE; }
"do"                    { new_token(DO); return DO; }
"double"                { new_token(DOUBLE); return DOUBLE; }
"else"                  { new_token(ELSE); return ELSE; }
"false"                 { new_token(FALSE); return FALSE; }
"float"                 { new_token(FLOAT); return FLOAT; }
"for"                   { new_token(FOR); return FOR; }
"if"                    { new_token(IF); return IF; }
"int"                   { new_token(INT); return INT; }
"long"                  { new_token(LONG); return LONG; }
"new"                   { new_token(NEW); return NEW; }
"private"               { new_token(PRIVATE); return PRIVATE; }
"protected"             { new_token(PROTECTED); return PROTECTED; }
"public"                { new_token(PUBLIC); return PUBLIC; }
"return"                { new_token(RETURN); return RETURN; }
"short"                 { new_token(SHORT); return SHORT; }
"signed"                { new_token(SIGNED); return SIGNED; }
"sizeof"                { new_token(SIZEOF); return SIZEOF; }
"struct"                { new_token(STRUCT); return STRUCT; }
"switch"                { new_token(SWITCH); return SWITCH; }
"true"                  { new_token(TRUE); return TRUE; }
"unsigned"              { new_token(UNSIGNED); return UNSIGNED; }
"void"                  { new_token(VOID); return VOID; }
"while"                 { new_token(WHILE); return WHILE; }

  /* constants */
{D}+{IS}?               { new_token(INTEGER); return INTEGER; }
{D}+{FS}?               { new_token(FLOATING); return FLOATING; }
{D}*"."{D}+{FS}?        { new_token(FLOATING); return FLOATING; }
{D}+"."{D}*{FS}?        { new_token(FLOATING); return FLOATING; }

\'                      { new_token(CHARACTER); BEGIN(CHR); }
<CHR>{
	\'              { yyerror("Empty char literal"); }
	"\\'"           { yytoken->ival = '\''; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\\""          { yytoken->ival = '"';  token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\?"           { yytoken->ival = '\?'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\a"           { yytoken->ival = '\a'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\b"           { yytoken->ival = '\b'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\f"           { yytoken->ival = '\f'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\n"           { yytoken->ival = '\n'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\r"           { yytoken->ival = '\r'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\t"           { yytoken->ival = '\t'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\v"           { yytoken->ival = '\v'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\0"           { yytoken->ival = '\0'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	"\\\\"          { yytoken->ival = '\\'; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	[^\\"'"]        { yytoken->ival = *yytext; token_append_text(yytoken, yytext); BEGIN(CHREND); }
	.               { yyerror("Unrecognized token in char literal"); }
}
<CHREND>{
	\'              { token_append_text(yytoken, yytext); BEGIN(INITIAL); return CHARACTER; }
	\n              { yyerror("Unexpected newline in char literal"); }
	.               { yyerror("Too many symbols in char literal"); }
}


L?\"                    { new_token(STRING); BEGIN(STR); }
<STR>{
	\"              { token_finish_sval(yytoken); token_append_text(yytoken, yytext); BEGIN(INITIAL); return STRING; }
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
	"\\0"           { token_append_sval_char(yytoken, '\0'); token_append_text(yytoken, yytext); }
	"\\\\"          { token_append_sval_char(yytoken, '\\'); token_append_text(yytoken, yytext); }
	[^\\\"\n]+      { token_append_sval_string(yytoken, yytext); token_append_text(yytoken, yytext); }
	\n              { yyerror("Unexpected newline in string literal"); }
	.               { yyerror("Unrecognized token in string literal"); }
	<<EOF>>         { yyerror("Unterminated string literal"); }
}

  /* operators */
">>="                   { new_token(SREQ); return SREQ; }
"<<="                   { new_token(SLEQ); return SLEQ; }
"+="                    { new_token(ADDEQ); return ADDEQ; }
"-="                    { new_token(SUBEQ); return SUBEQ; }
"*="                    { new_token(MULEQ); return MULEQ; }
"/="                    { new_token(DIVEQ); return DIVEQ; }
"%="                    { new_token(MODEQ); return MODEQ; }
"&="                    { new_token(ANDEQ); return ANDEQ; }
"^="                    { new_token(XOREQ); return XOREQ; }
"|="                    { new_token(OREQ); return OREQ; }
">>"                    { new_token(SR); return SR; }
"<<"                    { new_token(SL); return SL; }
"++"                    { new_token(PLUSPLUS); return PLUSPLUS; }
"--"                    { new_token(MINUSMINUS); return MINUSMINUS; }
"->"                    { new_token(ARROW); return ARROW; }
"->*"                   { new_token(ARROWSTAR); return ARROWSTAR; }
"&&"                    { new_token(ANDAND); return ANDAND; }
"||"                    { new_token(OROR); return OROR; }
"<="                    { new_token(LTEQ); return LTEQ; }
">="                    { new_token(GTEQ); return GTEQ; }
"=="                    { new_token(EQ); return EQ; }
"!="                    { new_token(NOTEQ); return NOTEQ; }
";"                     { new_token(';'); return ';'; }
"{"                     { new_token('{'); return '{'; }
"}"                     { new_token('}'); return '}'; }
","                     { new_token(','); return ','; }
"::"                    { new_token(COLONCOLON); return COLONCOLON; }
":"                     { new_token(':'); return ':'; }
"="                     { new_token('='); return '='; }
"("                     { new_token('('); return '('; }
")"                     { new_token(')'); return ')'; }
"["                     { new_token('['); return '['; }
"]"                     { new_token(']'); return ']'; }
"."                     { new_token('.'); return '.'; }
"&"                     { new_token('&'); return '&'; }
"!"                     { new_token('!'); return '!'; }
"~"                     { new_token('~'); return '~'; }
"-"                     { new_token('-'); return '-'; }
"+"                     { new_token('+'); return '+'; }
"*"                     { new_token('*'); return '*'; }
"/"                     { new_token('/'); return '/'; }
"%"                     { new_token('%'); return '%'; }
"<"                     { new_token('<'); return '<'; }
">"                     { new_token('>'); return '>'; }
"^"                     { new_token('^'); return '^'; }
"|"                     { new_token('|'); return '|'; }
"?"                     { new_token('?'); return '?'; }

{L}({L}|{D})*           { return check_identifier(yytext); }

<*>.                    { yyerror("Unrecognized token"); }

<<EOF>>                 {
	/* pop the current buffer and filename */
	yypop_buffer_state();
	list_pop(filenames);

	/* if there's another file to process, chdir to it */
	char *filename = current_filename();
	if (filename) {
		chdir(dirname(filename));
		free(filename);
	}

	yylineno = 1; /* restart line numbering count */

	if (!YY_CURRENT_BUFFER) /* if buffer stack is empty, stop */
		yyterminate();
  }
%%

void handle_include(const char *s)
{
	/* get the include path */
	size_t len = strlen(s) - 2; /* size without surrounding quotes */
	char *include = calloc(len+1, sizeof(char));
	if (include == NULL)
		handle_error("handle include buffer");
	strncpy(include, s+1, len); /* copy substring */
	include[len] = '\0';

	/* get the real path */
	char *filename = realpath(include, NULL);
	if (filename == NULL) {
		fprintf(stderr, "Could not resolve path to included file: %s, included from %s\n", include, current_filename());
		exit(EXIT_FAILURE);
	}
	free(include);
	list_push(filenames, filename);

	/* open file and push buffer */
	yyin = fopen(filename, "r");
	if (yyin == NULL) {
		fprintf(stderr, "Could not open included file: %s, include from %s\n", filename, current_filename());
		exit(EXIT_FAILURE);
	}
	yylineno = 1;
	yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

	return;
}
