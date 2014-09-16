/*
 * main.c - Accepts files for the 120++ lexical analyzer.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "clex.h"
#include "token.h"
#include "list.h"
#include "cgram.tab.h"

struct token *yytoken = NULL;
struct list *tokens = NULL;
struct list *filenames = NULL;

/* multiple error labels were redundant when all we can do is exit */
void handle_error(char *c)
{
	perror(c);
	exit(EXIT_FAILURE);
}

char *current_filename()
{
	const char *filename = list_peek(filenames);
	if (filename == NULL)
		return NULL;
	char *copy = strdup(filename);
	if (copy == NULL)
		handle_error("current_filename()");
	return copy;
}

/* start the parser via yylex; error on relevant tokens; add good
   tokens to token list */
void parse_files()
{
	while (true) {
		int category = yylex();
		if (category == 0) {
			break;
		} else if (category < BEGTOKEN || category > ENDTOKEN ) {
			fprintf(stderr, "Unkown return value from yylex %d\n", category);
			exit(EXIT_FAILURE);
		}
		list_push(tokens, yytoken);
	}
	yylex_destroy();
	return;
}

int main(int argc, char **argv)
{
	tokens = list_init();
	filenames = list_init();
	if (tokens == NULL)
		handle_error("main tokens");
	if (filenames == NULL)
		handle_error("main filenames");

	if (argc == 1) {
		/* not 'char filename[] = "stdin"' because list_destroy */
		char *filename = strdup("stdin");
		if (filename == NULL)
			handle_error("main filename");
		list_push(filenames, filename);
		yyin = stdin;
		yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
	} else {
		for (int i = 1; i < argc; ++i) {
			/* get real path for argument */
			char *filename = realpath(argv[i], NULL);
			if (filename == NULL)
				handle_error("main real path");

			/* open file and push filename and buffer */
			list_push(filenames, filename);
			yyin = fopen(filename, "r");
			if (yyin == NULL)
				handle_error("main file open");
			yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
		}
		/* start is a special case where we need to chdir to
		   the directory of the first file to be parsed; all
		   other files are chdir'ed to after the previous has
		   been popped */
		char *filename = current_filename();
		if (filename) {
			chdir(dirname(filename));
			free(filename);
		}
	}

	parse_files();

	printf("Line/Filename    Token   Text -> Ival/Sval\n");
	printf("------------------------------------------\n");
	const struct list_node *iter = list_head(tokens);
	while (!list_end(iter)) {
		const struct token *t = iter->data;

		char *filename = strdup(t->filename);
		if (filename == NULL)
			handle_error("main token filename");

		printf("%-5d%-12s%-8d%s ",
		       t->lineno,
		       basename(filename),
		       t->category,
		       t->text);

		free(filename);

		if (t->category == ICON)
			printf("-> %d", t->ival);
		else if (t->category == FCON)
			printf("-> %f", t->fval);
		else if (t->category == CCON)
			printf("-> %c", t->ival);
		else if (t->category == SCON)
			printf("-> %s", t->sval);

		printf("\n");
		iter = iter->next;
	}

	return EXIT_SUCCESS;
}
