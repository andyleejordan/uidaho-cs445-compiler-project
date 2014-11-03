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
#include <argp.h>

#include "args.h"
#include "logger.h"

#include "libs.h"
#include "lexer.h"

#include "list.h"
#include "tree.h"
#include "hasht.h"

/* argument parser */
const char *argp_program_version = "120++ hw3";
const char *argp_program_bug_address = "<andrew@schwartzmeyer.com>";
static char doc[] = "University of Idaho CS 445 - Compiler and Translator design: 120++ compiler";
static char args_doc[] = "120 [-tsc] infile...";

static struct argp_option options[] = {
	{ "debug",   'd', 0,      0, "Call perror() when crashing" },
	{ "tree",    't', 0,      0, "Print the syntax tree" },
	{ "symbols", 's', 0,      0, "Print the populated symbols" },
	{ "checks",  'c', 0,      0, "Print the performed type checks" },
	{ 0 }
};

static error_t parse_opt(int key, char *arg, struct argp_state *state);
static struct argp argp = { options, parse_opt, args_doc, doc };

/* shared with lexer and parser */
struct tree *yyprogram;
struct list *filenames;

/* chdir to dirname of given filename safely */
void chdirname(char *c);

/* from lexer */
extern struct hasht *typenames;
void free_typename(struct hash_node *t);

/* from parser */
int yyparse();
bool print_tree(struct tree *t, int d);

/* from symbol */
struct hasht *symbol_populate(struct tree *syntax);

int main(int argc, char **argv)
{
	arguments.debug = false;
	arguments.tree = false;
	arguments.symbols = false;
	arguments.checks = false;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	char *cwd = getcwd(NULL, 0);
	log_assert(cwd);

	filenames = list_new(NULL, &free);
	log_assert(filenames);

	/* setup lexer and parse each argument (or stdin) as a new 'program' */
	for (int i = 0; arguments.input_files[i]; ++i) {
		/* reset library flags */
		libs.usingstd	= false;
		libs.cstdlib	= false;
		libs.cmath	= false;
		libs.ctime	= false;
		libs.cstring	= false;
		libs.fstream	= false;
		libs.iostream	= false;
		libs.string	= false;
		libs.iomanip	= false;

		/* reset directory for each input file */
		log_assert(chdir(cwd) == 0);

		/* setup typenames table for lexer */
		typenames = hasht_new(8, true, NULL, NULL, &free_typename);
		log_assert(typenames);

		/* resolve path to input file */
		char *filename = realpath(arguments.input_files[i], NULL);
		if (filename == NULL)
			log_error("could not find input file: %s", arguments.input_files[i]);

		printf("parsing file: %s\n", filename);

		/* chdir for relative path lookups */
		chdirname(filename);

		/* open file and push buffer for flex */
		yyin = fopen(filename, "r");
		if (yyin == NULL)
			log_error("could not open input file: %s", filename);

		/* push filename and buffer state for lexer */
		list_push_back(filenames, filename);
		yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

		log_debug("invoking Bison");
		int result = yyparse();
		if (result != 0)
			return 2;

		/* print syntax tree */
		if (arguments.tree)
			tree_preorder(yyprogram, 0, &print_tree);

		/* build the symbol tables */
		struct hasht *global = symbol_populate(yyprogram);
		log_debug("global scope had %zu symbols", hasht_used(global));

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
	log_assert(filename);

	char *dir = dirname(filename);
	log_assert(dir);

	if (chdir(dir) != 0)
		log_error("could not chdir: %s", dir);

	free(filename);
}

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch (key) {
	case 'd':
		arguments->debug = true;
		break;
	case 't':
		arguments->tree = true;
		break;
	case 's':
		arguments->symbols = true;
		break;
	case 'c':
		arguments->checks = true;
		break;

	case ARGP_KEY_NO_ARGS:
		argp_usage(state);

	case ARGP_KEY_ARGS:
		arguments->input_files = &state->argv[state->next];
		state->next = state->argc;
		break;

	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}
