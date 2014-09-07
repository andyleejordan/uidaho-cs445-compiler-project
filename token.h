#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

struct token
{
	int category;
	int lineno;
	char *text;
	char *filename;
	int ival;
	char *sval;
};

/* malloc token and assign values */
struct token *token_create(int category, int lineno, char *text, char* filename);

/* free internal values */
void token_free(struct token *t);

/* append char literal to yytoken->sval string (to parse escapes) */
void token_append_sval_char(struct token *t, char c);

/* append string literal to yytoken->sval string (with null) */
void token_append_sval_string(struct token *t, char *s);

/* append string literal yytoken->text (with null) */
void token_append_text(struct token *t, char* s);

#endif /* TOKEN_H */
