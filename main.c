/*
 * main.c - Accepts files for the 120++ compiler.
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
#include "logger.h"

#include "list.h"
#include "tree.h"
#include "hasht.h"

#include "lexer.h"

/* from lexer */
extern struct hasht *typenames;
void free_typename(struct hash_node *t);

/* from parser */
int yyparse();
bool print_tree(struct tree *t, int d);

/* from symbol */
struct hasht *symbol_populate(struct tree *syntax);

/* shared with lexer and parser */
struct tree *yyprogram = NULL;
struct list *filenames = NULL;

/* chdir to dirname of given filename safely */
void chdirname(char *c);

int main(int argc, char **argv)
{
	char *cwd = getcwd(NULL, 0);
	if (cwd == NULL)
		log_crash();

	filenames = list_new(NULL, &free);
	if (filenames == NULL)
		log_crash();

	/* setup lexer and parse each argument (or stdin) as a new 'program' */
	for (int i = 1; i <= argc; ++i) {
		typenames = hasht_new(2, true, NULL, NULL, &free_typename);
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
			printf("Reading from argument %d: %s\n", i, argv[i]);

			/* get real path for argument */
			if (chdir(cwd) != 0)
				handle_error("Could not chdir to start dir");
			filename = realpath(argv[i], NULL);
			if (filename == NULL) {
				sprintf(error_buf, "Could not resolve CLI argument '%s'", argv[i]);
				handle_error(error_buf);
			}
			/* for relative path lookups */
			chdirname(filename);

			/* open file and push buffer for flex */
			if ((yyin = fopen(filename, "r")) == NULL) {
				sprintf(error_buf, "Could not open CLI argument '%s'", argv[i]);
				handle_error(error_buf);
			}
		}

		/* push filename and buffer state for lexer */
		list_push_back(filenames, filename);
		yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

		log_debug("Invoking Bison");
		int result = yyparse();
		if (result != 0)
			return 2;

		/* print syntax tree */
		tree_preorder(yyprogram, 0, &print_tree);

		/* build the symbol tables */
		struct hasht *global = symbol_populate(yyprogram);
		log_debug("global symbol table had %zu entries", hasht_used(global));

		/* clean up */
		tree_free(yyprogram);
		yylex_destroy();
		hasht_free(typenames);
	}

	list_free(filenames);

	return EXIT_SUCCESS;
}

/*
 * Helper function to safely chdir to dirname of given filename.
 */
void chdirname(char *c)
{
	char *filename = strdup(c);
	if (filename == NULL)
		log_crash();

	char *dir = dirname(filename);
	if (dir == NULL)
		log_crash();

	if (chdir(dir) != 0)
		log_error("Could not chdir to %s", dir);

	free(filename);
}
