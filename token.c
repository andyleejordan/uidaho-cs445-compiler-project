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
#include <libgen.h>

#include "token.h"
#include "logger.h"
#include "parser.tab.h"

static const int TEXT_CHUNK_SIZE = 128;

static size_t token_sval_size;

static char *print_category(int t);
static void token_realloc_sval(struct token *t);
static void token_realloc_text(struct token *t, const char *s);

/* malloc token and assign values */
struct token *token_new(int category, int lineno,
                        const char *text, const char* filename)
{
	struct token *t = malloc(sizeof(*t));
	if (t == NULL)
		log_error("token_new(): could not malloc token");

	t->category = category;
	t->lineno = lineno;

	t->text = strdup(text);

	t->filename = strdup(filename);

	if (category == INTEGER)
		t->ival = atoi(text);

	if (category == FLOATING)
		t->fval = atof(text);

	if (category == STRING) {
		t->ssize = 0; /* append null later */
		token_sval_size = TEXT_CHUNK_SIZE;
		t->sval = calloc(token_sval_size, sizeof(char));
	}

	return t;
}

/* free internal values */
void token_free(struct token *t)
{
	log_assert(t);
	free(t->text);
	free(t->filename);
	if (t->category == STRING)
		free(t->sval);
	free(t);
}

/* print a token */
void token_print(struct token *t)
{
		char *filename = strdup(t->filename);
		log_assert(filename);

		printf("%-5d%-12s%-12s%s ",
		       t->lineno,
		       basename(filename),
		       print_category(t->category),
		       t->text);

		free(filename);

		if (t->category == INTEGER)
			printf("-> %d", t->ival);
		else if (t->category == FLOATING)
			printf("-> %f", t->fval);
		else if (t->category == CHARACTER)
			printf("-> %c", t->ival);
		else if (t->category == STRING)
			printf("-> %s", t->sval);

		printf("\n");
}

/*
 * append a single char to sval (used for processing escape codes)
 */
void token_push_sval_char(struct token *t, char c)
{
	++t->ssize;
	token_realloc_sval(t);
	t->sval[t->ssize-1] = c; /* 0 indexed */
}

/*
 * append a string (without escape codes) to sval
 *
 * memcpy is used over strcat because the current value of sval is not
 * guaranteed to be a null-terminated string, and may very well have
 * embedded null characters
 */
void token_push_sval_string(struct token *t, const char *s)
{
	size_t end = t->ssize;
	t->ssize += strlen(s);
	token_realloc_sval(t);
	memcpy(t->sval + end, s, strlen(s));
}

/*
 * called at end of string pattern
 *
 * Resets token_sval_size to 0. Appends terminating null
 * character. Shrinks string with realloc to recorded length of
 * string. 'strlen' cannot be used because of potentially embedded
 * null charaters.
 */
void token_finish_sval(struct token *t)
{
	token_sval_size = 0;
	++t->ssize;
	token_push_sval_char(t, '\0');
	t->sval = realloc(t->sval, t->ssize);
	log_assert(t->sval);
}

/*
 * append string to text field (these are actual null terminated
 * strings, so the issues with sval do not apply)
 */
void token_push_text(struct token *t, const char* s)
{
	token_realloc_text(t, s);
	strcat(t->text, s);
}

/*
 * reallocate t->sval with additional chunks of memory
 *
 * "If the new size you specify is the same as the old size, realloc
 * is guaranteed to change nothing and return the same address that
 * you gave." - section 3.2.2.4 of libc manual
 *
 * Therefore this function is idempotent if another chunk of memory is
 * not required; otherwise, it increments it exactly as needed.
 */
static void token_realloc_sval(struct token *t)
{
	while (t->ssize > token_sval_size)
		token_sval_size += TEXT_CHUNK_SIZE;

	t->sval = realloc(t->sval, token_sval_size);
	log_assert(t->sval);
}

/*
 * reallocate text field to append string and terminating null
 */
static void token_realloc_text(struct token *t, const char *s)
{
	size_t size = strlen(t->text)+strlen(s)+1;
	t->text = realloc(t->text, size);
	log_assert(t->text);
}


#define R(rule) case rule: return #rule
static char *print_category(int t)
{
	switch(t) {
		R(IDENTIFIER);
		R(INTEGER);
		R(FLOATING);
		R(CHARACTER);
		R(STRING);
		R(CLASS_NAME);
		R(COLONCOLON);
		R(DOTSTAR);
		R(ADDEQ);
		R(SUBEQ);
		R(MULEQ);
		R(DIVEQ);
		R(MODEQ);
		R(XOREQ);
		R(ANDEQ);
		R(OREQ);
		R(SL);
		R(SR);
		R(SREQ);
		R(SLEQ);
		R(EQ);
		R(NOTEQ);
		R(LTEQ);
		R(GTEQ);
		R(ANDAND);
		R(OROR);
		R(PLUSPLUS);
		R(MINUSMINUS);
		R(ARROWSTAR);
		R(ARROW);
		R(BOOL);
		R(BREAK);
		R(CASE);
		R(CHAR);
		R(CLASS);
		R(CONTINUE);
		R(DEFAULT);
		R(DELETE);
		R(DO);
		R(DOUBLE);
		R(ELSE);
		R(FALSE);
		R(FLOAT);
		R(FOR);
		R(IF);
		R(INT);
		R(LONG);
		R(NEW);
		R(PRIVATE);
		R(PROTECTED);
		R(PUBLIC);
		R(RETURN);
		R(SHORT);
		R(SIGNED);
		R(SIZEOF);
		R(STRUCT);
		R(SWITCH);
		R(TRUE);
		R(UNSIGNED);
		R(VOID);
		R(WHILE);
	}

	return NULL; /* error */
}
#undef R
