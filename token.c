/*
 * token.c - Source code for tokens.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "token.h"
#include "cgram.tab.h"

struct token *token_create(int category, int lineno,
                           const char *text, const char* filename)
{
	struct token *t = malloc(sizeof(*t));

	t->category = category;
	t->lineno = lineno;

	t->text = calloc(strlen(text)+1, sizeof(char));
	strcpy(t->text, text);

	t->filename = calloc(strlen(filename)+1, sizeof(char));
	strcpy(t->filename, filename);

	if (category == ICON)
		t->ival = atoi(text);

	if (category == FCON)
		t->fval = atof(text);

	if (category == SCON)
		t->sval = calloc(128, sizeof(char));

	return t;
}

void token_free(struct token *t)
{
	free(t->text);
	free(t->filename);
	if (t->category == SCON)
		free(t->sval);
	free(t);
}

/* reallocate t->sval to append string and terminating null */
void token_realloc_sval(struct token *t, const char *s)
{
	size_t size = strlen(t->sval)+strlen(s)+1;
	t->sval = realloc(t->sval, size);
	if (t->sval == NULL)
		goto error;
	return;
 error:
	perror("token_realloc_sval()");
	exit(EXIT_FAILURE);
}

void token_append_sval_char(struct token *t, char c)
{
	size_t len = strlen(t->sval);
	token_realloc_sval(t, &c);
	t->sval[len] = c;
}

void token_append_sval_string(struct token *t, const char *s)
{
	token_realloc_sval(t, s);
	strcat(t->sval, s);
}

/* reallocate yytoken->text to append string and terminating null */
void token_realloc_text(struct token *t, const char *s)
{
	size_t size = strlen(t->text)+strlen(s)+1;
	t->text = realloc(t->text, size);
	if (t->text == NULL)
		goto error;
	return;
 error:
	perror("token_realloc_text()");
	exit(EXIT_FAILURE);
}

void token_append_text(struct token *t, const char* s)
{
	token_realloc_text(t, s);
	strcat(t->text, s);
}
