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
#include <ctype.h>
#include <libgen.h>

#include "list.h"
#include "tree.h"
#include "hasht.h"
#include "token.h"
#include "parser.tab.h"

/* syntactic action helpers */
#define TOKEN(name) do { prepare_token(name); return name; } while(0)
#define YYAPPENDTEXT() token_push_text(yytoken, yytext)
#define YYAPPENDCHAR(character) token_push_sval_char(yytoken, character)

/* from main */
extern struct list *filenames;
extern char* error_buf;

void chdirname(char *c);
void handle_error(char *c);

/* from parser */
void delete_tree(void *data, bool leaf);

/* creation of tokens */
struct token *yytoken = NULL;
void prepare_token(int category);

/* handle #include libraries */
bool usingstd = false;
bool cstdlib  = false;
bool cmath    = false;
bool ctime    = false;
bool cstring  = false;
bool fstream  = false;
bool iostream = false;
bool string   = false;
bool iomanip  = false;

void handle_fstream();
void handle_string();

/* handle #include files */
void handle_include(const char *s);

/* typenames data */
struct hasht *typenames;
void insert_typename(char *k, int c);
void insert_typename_tree(struct tree *t, int category);
int check_identifier(const char *s);

/* error helpers */
void lexerror(const char *s);
void unsupported();

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
        "<cstdlib>"     { cstdlib  = true; BEGIN(INITIAL); }
        "<cmath>"       { cmath    = true; BEGIN(INITIAL); }
        "<ctime>"       { ctime    = true; BEGIN(INITIAL); }
        "<cstring>"     { cstring  = true; BEGIN(INITIAL); }
        "<fstream>"     { fstream  = true;
                          if (usingstd)
                                  handle_fstream();
                          BEGIN(INITIAL); }
        "<iostream>"    { iostream = true; BEGIN(INITIAL); }
        "<string>"      { string   = true;
                          if (usingstd)
                                  handle_string();
                          BEGIN(INITIAL); }
        "<iomanip>"     { iomanip  = true; BEGIN(INITIAL); }
        \"[^\"]+\"      { handle_include(yytext); BEGIN(INITIAL); }
        "<"[^<>]*">"    { lexerror("Unrecognized system library"); }
        <<EOF>>         { lexerror("Unexpected EOF"); }
        .               { lexerror("Unrecognized token following #include"); }
}

  /* only allowed namespace directive */
"using namespace std;"  { usingstd = true;
                          if (fstream)
                                  handle_fstream();
                          if (string)
                                  handle_string(); }

  /* keywords */
"bool"                  { TOKEN(BOOL); }
"break"                 { TOKEN(BREAK); }
"case"                  { TOKEN(CASE); }
"char"                  { TOKEN(CHAR); }
"class"                 { TOKEN(CLASS); }
"continue"              { TOKEN(CONTINUE); }
"default"               { TOKEN(DEFAULT); }
"delete"                { TOKEN(DELETE); }
"do"                    { TOKEN(DO); }
"double"                { TOKEN(DOUBLE); }
"else"                  { TOKEN(ELSE); }
"false"                 { TOKEN(FALSE); }
"float"                 { TOKEN(FLOAT); }
"for"                   { TOKEN(FOR); }
"if"                    { TOKEN(IF); }
"int"                   { TOKEN(INT); }
"long"                  { TOKEN(LONG); }
"new"                   { TOKEN(NEW); }
"private"               { TOKEN(PRIVATE); }
"protected"             { TOKEN(PROTECTED); }
"public"                { TOKEN(PUBLIC); }
"return"                { TOKEN(RETURN); }
"short"                 { TOKEN(SHORT); }
"signed"                { TOKEN(SIGNED); }
"sizeof"                { TOKEN(SIZEOF); }
"struct"                { TOKEN(STRUCT); }
"switch"                { TOKEN(SWITCH); }
"true"                  { TOKEN(TRUE); }
"unsigned"              { TOKEN(UNSIGNED); }
"void"                  { TOKEN(VOID); }
"while"                 { TOKEN(WHILE); }

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
"volatile"              { unsupported(); }

  /* integer and floating constants */
{D}+{IS}?               { TOKEN(INTEGER); }
{D}+{FS}?               { TOKEN(FLOATING); }
{D}*"."{D}+{FS}?        { TOKEN(FLOATING); }
{D}+"."{D}*{FS}?        { TOKEN(FLOATING); }

  /* character literal */
\'                      { prepare_token(CHARACTER); BEGIN(CHR); }

<CHR>{
        \'              { lexerror("Empty char literal"); }
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
        .               { lexerror("Unrecognized token in char literal"); }
}

<CHREND>{
        \'              { YYAPPENDTEXT(); BEGIN(INITIAL); return CHARACTER; }
        \n              { lexerror("Unexpected newline in char literal"); }
        .               { lexerror("Too many symbols in char literal"); }
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
        \n              { lexerror("Unexpected newline in string literal"); }
        .               { lexerror("Unrecognized token in string literal"); }
        <<EOF>>         { lexerror("Unterminated string literal"); }
}

  /* operators */
">>="                   { TOKEN(SREQ); }
"<<="                   { TOKEN(SLEQ); }
"+="                    { TOKEN(ADDEQ); }
"-="                    { TOKEN(SUBEQ); }
"*="                    { TOKEN(MULEQ); }
"/="                    { TOKEN(DIVEQ); }
"%="                    { TOKEN(MODEQ); }
"&="                    { TOKEN(ANDEQ); }
"^="                    { TOKEN(XOREQ); }
"|="                    { TOKEN(OREQ); }
">>"                    { TOKEN(SR); }
"<<"                    { TOKEN(SL); }
"++"                    { TOKEN(PLUSPLUS); }
"--"                    { TOKEN(MINUSMINUS); }
"->"                    { TOKEN(ARROW); }
"->*"                   { TOKEN(ARROWSTAR); }
"&&"                    { TOKEN(ANDAND); }
"||"                    { TOKEN(OROR); }
"<="                    { TOKEN(LTEQ); }
">="                    { TOKEN(GTEQ); }
"=="                    { TOKEN(EQ); }
"!="                    { TOKEN(NOTEQ); }
";"                     { TOKEN(';'); }
"{"                     { TOKEN('{'); }
"}"                     { TOKEN('}'); }
","                     { TOKEN(','); }
"::"                    { TOKEN(COLONCOLON); }
":"                     { TOKEN(':'); }
"="                     { TOKEN('='); }
"("                     { TOKEN('('); }
")"                     { TOKEN(')'); }
"["                     { TOKEN('['); }
"]"                     { TOKEN(']'); }
"."                     { TOKEN('.'); }
"&"                     { TOKEN('&'); }
"!"                     { TOKEN('!'); }
"~"                     { TOKEN('~'); }
"-"                     { TOKEN('-'); }
"+"                     { TOKEN('+'); }
"*"                     { TOKEN('*'); }
"/"                     { TOKEN('/'); }
"%"                     { TOKEN('%'); }
"<"                     { TOKEN('<'); }
">"                     { TOKEN('>'); }
"^"                     { TOKEN('^'); }
"|"                     { TOKEN('|'); }
"?"                     { TOKEN('?'); }

  /* identifer */
{L}({L}|{D})*           { return check_identifier(yytext); }

<*>.                    { lexerror("Unrecognized token"); }

<<EOF>>                 { /* pop the current buffer and filename */
                          yypop_buffer_state();
                          list_pop_back(filenames);

                          /* if there's another file to process, chdir to it */
                          char *filename = list_back(filenames);
                          if (filename != NULL)
                                  chdirname(filename);

                          yylineno = 1; /* restart line numbering count */

                          /* if buffer stack is empty, stop */
                          if (!YY_CURRENT_BUFFER)
                                  yyterminate(); }
%%

/*
 * Prints relevant information for lexical errors and exits returning 1
 * per assignment requirements.
 */
void lexerror(const char *s)
{
        fprintf(stderr, "Lexical error: file %s, line %d, token %s: %s\n",
                (const char *)list_back(filenames), yylineno, yytext, s);
        exit(1);
}

/*
 * Prints error message and exits per assignment requirements.
 */
void unsupported()
{
        fprintf(stderr, "C++ operation unsupported in 120++: file %s, line %d, token: %s\n",
                (const char *)list_back(filenames), yylineno, yytext);
        exit(3);
}

/*
 * Creates a token with the necessary information, then allocates a
 * tree node as a leaf for the token, saving it into yylval for Bison.
 */
void prepare_token(int category)
{
        yytoken = token_new(category, yylineno, yytext,
                            (const char *)list_back(filenames));
        yylval.t = tree_new(NULL, yytoken,
                            (void (*)(void *, bool))&delete_tree);
}

/*
 * Given yytext of the literal form "somefile.h", this extracts the
 * substring corresponding to the path, determines the full path to
 * the file, pushes that path to the filenames list, opens the file
 * into yyin, resets the line number count to 1, and pushes a new Flex
 * buffer for the file.
 */
void handle_include(const char *s)
{
        /* get the include path */
        size_t len = strlen(s) - 2; /* size without surrounding quotes */
        char *include = calloc(len+1, sizeof(char));
        if (include == NULL)
                handle_error("handle include");
        strncpy(include, s+1, len); /* copy substring */
        include[len] = '\0';

        /* get the real path */
        char *filename = realpath(include, NULL);
        if (filename == NULL) {
                sprintf(error_buf, "Could not resolve path to included file: %s, included from %s\n",
                        include, (const char *)list_back(filenames));
                handle_error(error_buf);
        }
        free(include);
        list_push_back(filenames, filename);

        /* open file and push buffer */
        yyin = fopen(filename, "r");
        if (yyin == NULL) {
                sprintf(error_buf, "Could not open included file: %s, include from %s\n",
                        filename, (const char *)list_back(filenames));
                handle_error(error_buf);
        }
        yylineno = 1;
        yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

        return;
}

/*
 * Insert "ifstream" and "ofstream" into typenames hash table.
 */
void handle_fstream()
{
	insert_typename("ifstream", CLASS_NAME);
	insert_typename("ofstream", CLASS_NAME);
}

/*
 * Insert "string" into typenames hash table.
 */
void handle_string()
{
	insert_typename("string", CLASS_NAME);
}

/*
 * Inserts typename into typenames hash table.
 *
 * Copies both the typename string (key) and integer category (value)
 * so that a) the table can be freed later b) the table is not
 * dependent on the source of the typename and c) the table wants
 * void*, not a plain int.
 */
void insert_typename(char *k, int c)
{
	char *key = strdup(k);
	int *i = malloc(sizeof(*i));
	*i = c;

	if (hasht_search(typenames, key))
                lexerror("token previously declared as identifier");

	hasht_insert(typenames, key, i);
}

/*
 * Unwraps a tree leaf and inserts token's text as key with category
 * as value into typenames hash table.
 */
void insert_typename_tree(struct tree *t, int category)
{
	char *key = ((struct token *)t->data)->text;
	insert_typename(key, category);
}

void free_typename(struct hash_node *t)
{
	free(t->key);
	free(t->value);
}

/*
 * Returns corresponding integer category for given identifier name
 * and creates the necessary token.
 */
int check_identifier(const char *s)
{
        int *c = hasht_search(typenames, (void *)s);
        if (c)
	        TOKEN(*c);
        else
	        TOKEN(IDENTIFIER);
}

#undef TOKEN
#undef YYAPPENDTEXT
#undef YYAPPENDCHAR
