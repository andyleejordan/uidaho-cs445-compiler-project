/*
 * lexer.l - Flex scanner for 120++.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

%option warn nounput noinput
%option header-file="lexer.h"
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
#include <libgen.h>

#include "args.h"
#include "node.h"
#include "logger.h"
#include "token.h"
#include "libs.h"
#include "parser.tab.h"
#include "rules.h"

#include "list.h"
#include "tree.h"
#include "hasht.h"

/* syntactic action helpers */
#define T(name) do { prepare_token(name); return name; } while(0)
#define YYAPPENDTEXT() token_push_text(yytoken, yytext)
#define YYAPPENDCHAR(character) token_push_sval_char(yytoken, character)

/* from main */
extern struct list *yyfiles;
extern struct hasht *yyincludes;
extern struct hasht *yytypes;

/* from parser */
void delete_tree(void *data, bool leaf);

/* creation of tokens */
static struct token *yytoken;
static void prepare_token(int category);

/* handle #include libraries */
static void handle_fstream();
static void handle_iostream();
static void handle_string();

/* handle #include files */
static void handle_include(char *filename);
static void handle_quoted_include(const char *s);

/* typenames data */
void insert_typename(char *k, int c);
void insert_typename_tree(struct tree *t, int category);
static int check_identifier(const char *s);

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

  /* includes */
"#include"              { BEGIN(INC); }

<INC>{
        [ \t]*          { /* eat whitespace */ }
        "<cstdlib>"     { libs.cstdlib  = true; BEGIN(INITIAL); }
        "<cmath>"       { libs.cmath    = true; BEGIN(INITIAL); }
        "<ctime>"       { libs.ctime    = true; BEGIN(INITIAL); }
        "<cstring>"     { libs.cstring  = true; BEGIN(INITIAL); }
        "<fstream>"     { libs.fstream  = true; handle_fstream(); BEGIN(INITIAL); }
        "<iostream>"    { libs.iostream = true; handle_iostream(); BEGIN(INITIAL); }
        "<string>"      { libs.string   = true; handle_string(); BEGIN(INITIAL); }
        "<iomanip>"     { libs.iomanip  = true; BEGIN(INITIAL); }
        \"[^\"]+\"      { handle_quoted_include(yytext); BEGIN(INITIAL); }
        "<"[^<>]*">"    { log_lexical("unrecognized library: %s", yytext); }
        <<EOF>>         { log_lexical("unexpected EOF"); }
        .               { log_lexical("unrecognized token: %s"); }
}

  /* only allowed namespace directive */
"using namespace std;"  { libs.usingstd = true; }

  /* keywords */
"bool"                  { T(BOOL); }
"break"                 { T(BREAK); }
"case"                  { T(CASE); }
"char"                  { T(CHAR); }
"class"                 { T(CLASS); }
"continue"              { T(CONTINUE); }
"default"               { T(DEFAULT); }
"delete"                { T(DELETE); }
"do"                    { T(DO); }
"double"                { T(DOUBLE); }
"else"                  { T(ELSE); }
"false"                 { T(FALSE); }
"float"                 { T(FLOAT); }
"for"                   { T(FOR); }
"if"                    { T(IF); }
"int"                   { T(INT); }
"long"                  { T(LONG); }
"new"                   { T(NEW); }
"private"               { T(PRIVATE); }
"protected"             { T(PROTECTED); }
"public"                { T(PUBLIC); }
"return"                { T(RETURN); }
"short"                 { T(SHORT); }
"signed"                { T(SIGNED); }
"sizeof"                { T(SIZEOF); }
"struct"                { T(STRUCT); }
"switch"                { T(SWITCH); }
"true"                  { T(TRUE); }
"unsigned"              { T(UNSIGNED); }
"void"                  { T(VOID); }
"while"                 { T(WHILE); }

  /* unsupported keywords */
"const"                 { /* const qualifier is discarded */ }
"auto"                  |
"..."                   |
"enum"                  |
"extern"                |
"friend"                |
"goto"                  |
"namespace"             |
"register"              |
"static"                |
"this"                  |
"typedef"               |
"union"                 |
"using"                 |
"virtual"               |
"volatile"              { log_unsupported(); }

  /* integer and floating constants */
{D}+{IS}?               { T(INTEGER); }
{D}+{FS}?               { T(FLOATING); }
{D}*"."{D}+{FS}?        { T(FLOATING); }
{D}+"."{D}*{FS}?        { T(FLOATING); }

  /* character literal */
\'                      { prepare_token(CHARACTER); BEGIN(CHR); }

<CHR>{
        \'              { log_lexical("empty char literal"); }
        "\\'"           { yytoken->ival = '\''; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\\""          { yytoken->ival = '"';  YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\?"           { yytoken->ival = '\?'; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\a"           { yytoken->ival = '\a'; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\b"           { yytoken->ival = '\b'; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\f"           { yytoken->ival = '\f'; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\n"           { yytoken->ival = '\n'; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\r"           { yytoken->ival = '\r'; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\t"           { yytoken->ival = '\t'; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\v"           { yytoken->ival = '\v'; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\0"           { yytoken->ival = '\0'; YYAPPENDTEXT(); BEGIN(CHREND); }
        "\\\\"          { yytoken->ival = '\\'; YYAPPENDTEXT(); BEGIN(CHREND); }
        [^\\"'"]        { yytoken->ival = *yytext;
                          YYAPPENDTEXT();
                          BEGIN(CHREND); }
        .               { log_lexical("in char literal, unrecognized token: %s", yytext); }
}

<CHREND>{
        \'              { YYAPPENDTEXT(); BEGIN(INITIAL); return CHARACTER; }
        \n              { log_lexical("in char literal: unexpected newline"); }
        .               { log_lexical("in char literal: too many symbols"); }
}

  /* string literal */
\"                      { prepare_token(STRING); BEGIN(STR); }

<STR>{
        \"              { token_finish_sval(yytoken);
                          YYAPPENDTEXT();
                          BEGIN(INITIAL);
                          return STRING; }
        "\\'"           { YYAPPENDCHAR('\''); YYAPPENDTEXT(); }
        "\\\""          { YYAPPENDCHAR('"');  YYAPPENDTEXT(); }
        "\\?"           { YYAPPENDCHAR('\?'); YYAPPENDTEXT(); }
        "\\a"           { YYAPPENDCHAR('\a'); YYAPPENDTEXT(); }
        "\\b"           { YYAPPENDCHAR('\b'); YYAPPENDTEXT(); }
        "\\f"           { YYAPPENDCHAR('\f'); YYAPPENDTEXT(); }
        "\\n"           { YYAPPENDCHAR('\n'); YYAPPENDTEXT(); }
        "\\r"           { YYAPPENDCHAR('\r'); YYAPPENDTEXT(); }
        "\\t"           { YYAPPENDCHAR('\t'); YYAPPENDTEXT(); }
        "\\v"           { YYAPPENDCHAR('\v'); YYAPPENDTEXT(); }
        "\\0"           { YYAPPENDCHAR('\0'); YYAPPENDTEXT(); }
        "\\\\"          { YYAPPENDCHAR('\\'); YYAPPENDTEXT(); }
        [^\\\"\n]+      { token_push_sval_string(yytoken, yytext);
                          YYAPPENDTEXT(); }
        \n              { log_lexical("in string literal: unexpected newline"); }
        .               { log_lexical("in string literal: unrecognized token"); }
        <<EOF>>         { log_lexical("in string literal: unterminated"); }
}

  /* operators */
">>="                   { T(SREQ); }
"<<="                   { T(SLEQ); }
"+="                    { T(ADDEQ); }
"-="                    { T(SUBEQ); }
"*="                    { T(MULEQ); }
"/="                    { T(DIVEQ); }
"%="                    { T(MODEQ); }
"&="                    { T(ANDEQ); }
"^="                    { T(XOREQ); }
"|="                    { T(OREQ); }
">>"                    { T(SR); }
"<<"                    { T(SL); }
"++"                    { T(PLUSPLUS); }
"--"                    { T(MINUSMINUS); }
"->"                    { T(ARROW); }
"->*"                   { T(ARROWSTAR); }
"&&"                    { T(ANDAND); }
"||"                    { T(OROR); }
"<="                    { T(LTEQ); }
">="                    { T(GTEQ); }
"=="                    { T(EQ); }
"!="                    { T(NOTEQ); }
";"                     { T(';'); }
"{"                     { T('{'); }
"}"                     { T('}'); }
","                     { T(','); }
"::"                    { T(COLONCOLON); }
":"                     { T(':'); }
"="                     { T('='); }
"("                     { T('('); }
")"                     { T(')'); }
"["                     { T('['); }
"]"                     { T(']'); }
"."                     { T('.'); }
"&"                     { T('&'); }
"!"                     { T('!'); }
"~"                     { T('~'); }
"-"                     { T('-'); }
"+"                     { T('+'); }
"*"                     { T('*'); }
"/"                     { T('/'); }
"%"                     { T('%'); }
"<"                     { T('<'); }
">"                     { T('>'); }
"^"                     { T('^'); }
"|"                     { T('|'); }
"?"                     { T('?'); }

  /* identifer */
{L}({L}|{D})*           { return check_identifier(yytext); }

<*>.                    { log_lexical("unrecognized token: %s", yytext); }

<<EOF>>                 { /* restart line numbering count */
                          yylineno = 1;
                          /* pop the current buffer and filename */
                          yypop_buffer_state();
                          list_pop_back(yyfiles);

                          /* if buffer stack is empty, stop */
                          if (!YY_CURRENT_BUFFER)
                                  yyterminate();
                        }
%%

/*
 * Creates a token with the necessary information, then allocates a
 * tree node as a leaf for the token, saving it into yylval for Bison.
 */
void prepare_token(int category)
{
	yytoken = token_new(category, yylineno, yytext,
                            (const char *)list_back(yyfiles));
	struct node *n = node_new(TOKEN);
	n->token = yytoken;
	yylval.t = tree_new(NULL, n, NULL,
	                    (void (*)(void *, bool))&delete_tree);
}

/*
 * Given yytext of the literal form "somefile.h", this extracts the
 * substring corresponding to the path, determines the full path to
 * the file, and sends the resolved path to the delegate function.
 */
static void handle_quoted_include(const char *s)
{
	/* size without surrounding quotes */
	size_t len = strlen(s) - 2;

	char *include = calloc(len + 1, sizeof(char));
	log_assert(include);

	/* copy substring */
	strncpy(include, s + 1, len);
	include[len] = '\0';

	/* path = realpath(dirname(current) + "/" + include) */
	char *current = list_back(yyfiles);

	/* copy because Wormulon's dirname modifies */
	char *copy = strdup(current);
	const char *dir = dirname(copy); /* do not pass to free */
	log_debug("dirname: %s", dir);

	/* append POSIX directory separator and include path */
	char *resolved;
	asprintf(&resolved, "%s/%s", dir, include);
	log_debug("resolved: %s", resolved);

	/* resolve abosolute path name */
	handle_include(resolved);

	free(copy);
	free(resolved);
	free(include);
}

/*
 * Given the path to a file, this pushes the path to the yyfiles list,
 * opens the file into yyin, resets the line number count to 1, and
 * pushes a new Flex buffer for the file.
 */
static void handle_include(char *s)
{
	/* restart line numbering count */
	yylineno = 1;

	char *current = list_back(yyfiles);

	char *filename = realpath(s, NULL);
	if (filename == NULL)
		log_error("could not find included file: %s\n"
		          "included from: %s", s, current);

	if (hasht_search(yyincludes, filename)) {
		log_debug("Flex: already included %s", filename);
		return;
	}

	/* open file */
	yyin = fopen(filename, "r");
	if (yyin == NULL)
		log_error("could not open included file: %s\n"
		          "included from: %s", filename, current);

	/* push filename and buffer state */
	list_push_back(yyfiles, filename);
	yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

	/* record filename in yyincludes set */
	hasht_insert(yyincludes, filename, filename);
	log_debug("filename: %s", filename);
}

/*
 * Insert C prototypes.
 */
void handle_c()
{
	char *path;
	asprintf(&path, "%s/include_c.h", arguments.include);
	handle_include(path);
	free(path);
}

/*
 * Insert "ifstream" and "ofstream" types.
 */
static void handle_fstream()
{
	char *path;
	asprintf(&path, "%s/include_fstream.h", arguments.include);
	handle_include(path);
	free(path);
}

/*
 * Insert "cin", "cout", and "endl" symbols.
 */
static void handle_iostream()
{
	char *path;
	asprintf(&path, "%s/include_iostream.h", arguments.include);
	handle_include(path);
	free(path);
}

/*
 * Insert "string" type.
 */
static void handle_string()
{
	char *path;
	asprintf(&path, "%s/include_string.h", arguments.include);
	handle_include(path);
	free(path);
}

/*
 * Inserts typename into yytypes hash table.
 *
 * Copies both the typename string (key) and integer category (value)
 * so that a) the table can be freed later b) the table is not
 * dependent on the source of the typename and c) the table wants
 * void*, not a plain int.
 */
void insert_typename(char *k, int c)
{
	log_debug("inserting typename %s", k);
	char *key = strdup(k);
	int *i = malloc(sizeof(*i));
	log_assert(key && i);

	*i = c;

	if (hasht_search(yytypes, key))
		log_lexical("typename %s previously declared", k);

	if (hasht_insert(yytypes, key, i) == NULL)
		log_error("failed to insert %s into yytypes table", k);
}

/*
 * Unwraps a tree leaf and inserts token's text as key with category
 * as value into yytypes hash table.
 */
void insert_typename_tree(struct tree *t, int category)
{
	struct node *node = t->data;
	log_assert(node);
	struct token *token = node->token;
	log_assert(token);
	char *key = token->text;
	insert_typename(key, category);
}

void free_typename(struct hasht_node *t)
{
	free(t->key);
	free(t->value);
}

/*
 * Returns corresponding integer category for given identifier name
 * and creates the necessary token.
 */
static int check_identifier(const char *s)
{
	int *c = hasht_search(yytypes, (void *)s);
	if (c)
		T(*c);
	else
		T(IDENTIFIER);
}

#undef T
#undef YYAPPENDTEXT
#undef YYAPPENDCHAR
