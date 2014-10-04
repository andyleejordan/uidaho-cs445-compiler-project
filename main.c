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
#include "tree.h"
#include "cgram.tab.h"

struct tree *yyprogram;
struct list *typenames = NULL;
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

void print_tree(struct tree *t)
{
	if (tree_size(t) == 0) /* holds a token */
		printf("%s\n", ((struct token*)t->data)->text);
	else /* holds a production rule name */
		printf("%s\n", t->data);
}

int main(int argc, char **argv)
{
	char *cwd = getcwd(NULL, 0);
	if (cwd == NULL)
		handle_error("getcwd");

	filenames = list_init();
	if (filenames == NULL)
		handle_error("main filenames");

	/* setup lexer and parse each argument (or stdin) as a new 'program' */
	for (int i = 1; i <= argc; ++i) {
		typenames = list_init();
		if (typenames == NULL)
			handle_error("main typenames");

		char *filename = NULL;
		if (argc == 1) {
			printf("No CLI arguments, reading from stdin\n");
			filename = "stdin";
			yyin = stdin;
		} else {
			if (i == argc)
				break;
			printf("Reading from %s\n", argv[i]);
			/* get real path for argument */
			chdir(cwd);
			filename = realpath(argv[i], NULL);
			if (filename == NULL) {
				sprintf(error_buf, "Could not resolve CLI argument '%s'", argv[i]);
				handle_error(error_buf);
			}
			chdir(dirname(filename)); /* relative path lookups */

			/* open file and push buffer for flex */
			if (!(yyin = fopen(filename, "r"))) {
				sprintf(error_buf, "Could not open CLI argument '%s'", argv[i]);
				handle_error(error_buf);
			}
		}

		/* push filename and buffer state for lexer */
		list_push(filenames, filename);
		yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

		yyparse(); /* call bison */

		tree_preorder(yyprogram, &print_tree);

		/* clean up */
		yylex_destroy();
		if (!fclose(yyin))
			handle_error("main fclose");
		list_destroy(typenames, NULL);
	}

	return EXIT_SUCCESS;
}
