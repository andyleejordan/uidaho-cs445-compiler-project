/*
 * logger.c - Logger implementation.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdarg.h>
#include <stdio.h>

#include "logger.h"
#include "args.h"

#include "token.h"
#include "lexer.h"

#include "list.h"
#include "tree.h"

/* from main */
extern struct list *filenames;

/*
 * Print message to stderr. If debug, call perror. Exit failure.
 */
void log_error(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);

	fprintf(stderr, "error: ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");

	va_end(ap);

	if (arguments.debug)
		perror("");

	exit(EXIT_FAILURE);
}

/*
 * If debug, print message to stderr. Continue.
 */
void log_debug(const char *format, ...)
{
	if (!arguments.debug)
		return;

	va_list ap;
	va_start(ap, format);

	fprintf(stderr, "debug: ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}

/*
 * Acts like assert.
 *
 * If errno is set, print it.
 */
void log_assert(bool p)
{
	if (p)
		return;

	fprintf(stderr, "assertion failed: probably received unexpected null\n");
	if (errno != 0)
		perror("");

	/* debug builds should allow the segfault for stack tracing */
	if (!arguments.debug)
		exit(EXIT_FAILURE);
}

/*
 * Prints error message and exits per assignment requirements.
 */
void log_unsupported()
{
	fprintf(stderr, "error: operation unsupported\n"
	        "file: %s\n" "line: %d\n" "token: %s\n",
                (const char *)list_back(filenames), yylineno, yytext);
	exit(3);
}

/*
 * Prints relevant information for lexical errors and exits returning 1
 * per assignment requirements.
 */
void log_lexical(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);

	fprintf(stderr, "lexical error: ");
	vfprintf(stderr, format, ap);

	va_end(ap);

	fprintf(stderr, "\n" "file: %s\n" "line: %d\n" "token: %s\n",
                (const char *)list_back(filenames), yylineno, yytext);

	exit(1);
}

/*
 * Follow a node to a token, emit error, exit with 3.
 */
void log_semantic(struct tree *n, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);

	fprintf(stderr, "semantic error: ");
	vfprintf(stderr, format, ap);

	va_end(ap);

	fprintf(stderr, "\n");
	if (n) {
		while (tree_size(n) > 1)
			n = list_front(n->children);
		struct token *t = n->data;
		fprintf(stderr, "file: %s\n" "line: %d\n" "token: %s\n",
		        t->filename, t->lineno, t->text);
	}

	exit(3);
}

void log_check(const char *format, ...)
{
	if (!arguments.checks)
		return;

	va_list ap;
	va_start(ap, format);

	fprintf(stderr, "type check: ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}
