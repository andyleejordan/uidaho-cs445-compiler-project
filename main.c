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

struct tree *yyprogram;
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

			/* call bison */
			yyparse();

			/* tree_print(program); */

			/* clean up */
			yylex_destroy();
		}

	}

	return EXIT_SUCCESS;
}
