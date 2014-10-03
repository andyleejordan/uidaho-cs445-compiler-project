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
#include <unistd.h>

#include "clex.h"
#include "token.h"
#include "list.h"
#include "cgram.tab.h"

struct tree *yyprogram;
struct list *tokens = NULL;
struct list *filenames = NULL;
char error_buf[256];

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
	char *cwd = getcwd(NULL, 0);
	if (cwd == NULL)
		handle_error("getcwd");

	tokens = list_init();
	filenames = list_init();
	if (tokens == NULL)
		handle_error("main tokens");
	if (filenames == NULL)
		handle_error("main filenames");

	if (argc == 1) { /* setup lexer and parse for stdin */
		printf("no CLI arguments, reading from stdin\n");
		list_push(filenames, "stdin");
		yyin = stdin;
		yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
	} else { /* setup lexer and parse each argument as a new 'program' */
		for (int i = 1; i < argc; ++i) {
			/* get real path for argument */
			chdir(cwd);
			char *filename = realpath(argv[i], NULL);
			if (filename == NULL) {
				sprintf(error_buf, "couldn't resolve CLI argument '%s'", argv[i]);
				handle_error(error_buf);
			}

			/* push to list for lexer */
			list_push(filenames, filename);

			/* change to directory for relative path lookups */
			chdir(dirname(filename));

			/* open file and push buffer for flex */
			yyin = fopen(filename, "r");
			if (yyin == NULL) {
				sprintf(error_buf, "couldn't open CLI argument '%s'", argv[i]);
				handle_error(error_buf);
			}
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
