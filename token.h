/*
 * token.h - Interface for tokens.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

/* interface from Dr. Jeffery */
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
struct token *token_create(int category, int lineno,
                           const char *text, const char* filename);

/* free internal values */
void token_free(struct token *t);

/* append char literal to yytoken->sval string (to parse escapes) */
void token_append_sval_char(struct token *t, char c);

/* append string literal to yytoken->sval string (with null) */
void token_append_sval_string(struct token *t, const char *s);

/* append string literal yytoken->text (with null) */
void token_append_text(struct token *t, const char* s);

#endif /* TOKEN_H */
