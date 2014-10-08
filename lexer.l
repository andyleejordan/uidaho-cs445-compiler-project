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
#include "token.h"
#include "parser.tab.h"

#define TOKEN(name) do { new_token(name); return name; } while(0)
#define YYAPPENDTEXT() token_append_text(yytoken, yytext)
#define YYAPPENDCHAR(character) token_append_sval_char(yytoken, character)

struct typename {
        char *name;
        int category;
};

extern struct list *typenames;
extern struct list *filenames;
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

void lexerror(const char *s);
void unsupported();
void new_token(int category);
void handle_include(const char *s);
void handle_fstream();
void handle_string();
bool typename_compare(const char *s, const struct typename *t);
void typenames_insert(char *s, int category);
void typenames_insert_tree(struct tree *t, int category);
int check_identifier(const char *s);

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
\'                      { new_token(CHARACTER); BEGIN(CHR); }

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
                          BEGIN(CHREND);
                        }
        .               { lexerror("Unrecognized token in char literal"); }
}

<CHREND>{
        \'              { YYAPPENDTEXT(); BEGIN(INITIAL); return CHARACTER; }
        \n              { lexerror("Unexpected newline in char literal"); }
        .               { lexerror("Too many symbols in char literal"); }
}

  /* string literal */
\"                      { new_token(STRING); BEGIN(STR); }

<STR>{
        \"              { token_finish_sval(yytoken);
                          YYAPPENDTEXT();
                          BEGIN(INITIAL);
                          return STRING;
                        }
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
        [^\\\"\n]+      { token_append_sval_string(yytoken, yytext);
                          YYAPPENDTEXT();
                        }
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

#undef TOKEN
#undef YYAPPENDTEXT
#undef YYAPPENDCHAR

/*
 * Prints relevant information for lexical errors and exits returning 1
 * per assignment requirements.
 */
void lexerror(const char *s)
{
        fprintf(stderr, "Lexical error: file %s, line %d, token %s: %s\n",
                (const char *)list_peek(filenames), yylineno, yytext, s);
        exit(1);
}

/*
 * Prints error message and exits per assignment requirements.
 */
void unsupported()
{
	fprintf(stderr, "C++ operation unsupported in 120++: file %s, line %d, token: %s\n",
	        (const char *)list_peek(filenames), yylineno, yytext);
	exit(3);
}

/*
 * Creates a token with the necessary information, then allocates a
 * tree node as a leaf for the token, saving it into yylval for Bison.
 */
void new_token(int category)
{
        yytoken = token_create(category, yylineno, yytext,
                                       (const char *)list_peek(filenames));
        yylval.t = tree_init(NULL, yytoken);
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

/*
 * Insert "ifstream" and "ofstream" into typenames list only if they
 * are not there already (sufficient to look for either).
 */
void handle_fstream()
{
        struct list_node *l = list_find(typenames, "ifstream",
                                        (bool (*)(void *, void *))&typename_compare);
        if (list_end(l)) {
                typenames_insert("ifstream", CLASS_NAME);
                typenames_insert("ofstream", CLASS_NAME);
        }
}

/*
 * Insert "string" into typenames list only if not already there.
 */
void handle_string()
{
        struct list_node *l = list_find(typenames, "string",
                                        (bool (*)(void *, void *))&typename_compare);
        if (list_end(l))
                typenames_insert("string", CLASS_NAME);
}

/*
 * Returns true if given name string matches the typename's name.
 */
bool typename_compare(const char *s, const struct typename *t)
{
        return 0 == strcmp(s, t->name);
}

/*
 * Given name string and integer category, inserts a new typename into
 * typenames list.
 */
void typenames_insert(char *s, int category)
{
        struct list_node *l = list_find(typenames, (void *)s,
                                        (bool (*)(void *, void *))&typename_compare);
        if (!list_end(l)) {
                lexerror("token previously declared as identifier");
        }
        struct typename *type = malloc(sizeof(*type));
        type->name = strdup(s);
        type->category = category;
        list_push(typenames, type);
}

/*
 * Unwraps a tree leaf and passes token text as name to insert
 * function.
 */
void typenames_insert_tree(struct tree *t, int category)
{
        typenames_insert(((struct token *)t->data)->text, category);
}

/*
 * Returns corresponding integer category for given identifier name
 * and creates the necessary token.
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
                int category = ((struct typename *)l->data)->category;
                new_token(category);
                return category;
        }
}
