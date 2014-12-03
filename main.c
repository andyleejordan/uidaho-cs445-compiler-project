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
#include <unistd.h>
#include <argp.h>

#include "args.h"
#include "logger.h"

#include "libs.h"
#include "lexer.h"
#include "symbol.h"
#include "node.h"
#include "intermediate.h"

#include "list.h"
#include "tree.h"
#include "hasht.h"

/* argument parser */
const char *argp_program_version = "120++ hw3";
const char *argp_program_bug_address = "<andrew@schwartzmeyer.com>";

static char doc[] = "Andrew Schwartzmeyer's 120++ compiler.\v"
	"University of Idaho - Department of Computer Science\n"
	"CS 445: Compiler and Translator Design, by Dr. Clinton Jeffery\n\n"
	"This is a work-in-progress old-school compiler using Flex and Bison\n"
	"for a subset of C++, dubbed '120++' as it should work for most code\n"
	"written in CS 120, including basic classes.\n\n"
	"This repo is located at: https://github.com/andschwa/uidaho-cs445";

static char args_doc[] = "infile...";

static struct argp_option options[] = {
	{ "debug",   'd', 0,      0, "Print debug messages (scopes, mostly).\n"
	  "Also disables exit on assertion failure."},
	{ "tree",    't', 0,      0, "Print the syntax tree." },
	{ "syntax",  't', 0,      OPTION_ALIAS },
	{ "symbols", 's', 0,      0, "Print the populated symbols." },
	{ "checks",  'c', 0,      0, "Print the performed type checks." },
	{ "types",   'c', 0,      OPTION_ALIAS },
	{ "include", 'I', "DIR",  0, "Search path for 'system' headers." },
	{ 0 }
};

static error_t parse_opt(int key, char *arg, struct argp_state *state);
static struct argp argp = { options, parse_opt, args_doc, doc };

/* shared with lexer and parser */
struct tree *yyprogram;
struct list *yyscopes;
struct list *yyfiles;
struct hasht *yyincludes;
struct hasht *yytypes;

enum region region;
size_t offset;

static void parse_program(char *filename);

/* from lexer */
void free_typename(struct hash_node *t);

/* from parser */
int yyparse();
bool print_tree(struct tree *t, int d);

int main(int argc, char **argv)
{
	arguments.debug = false;
	arguments.tree = false;
	arguments.symbols = false;
	arguments.checks = false;
	arguments.include = getcwd(NULL, 0);

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	/* parse each input file as a new 'program' */
	for (int i = 0; arguments.input_files[i]; ++i) {
		char *filename = realpath(arguments.input_files[i], NULL);
		if (filename == NULL)
			log_error("could not find input file: %s", arguments.input_files[i]);
		parse_program(filename);
	}

	return EXIT_SUCCESS;
}

void parse_program(char *filename)
{
	printf("parsing file: %s\n", filename);

	yyfiles = list_new(NULL, &free);
	log_assert(yyfiles);
	list_push_back(yyfiles, filename);

	yyincludes = hasht_new(8, true, NULL, NULL, NULL);
	log_assert(yyincludes);

	/* open file for lexer */
	yyin = fopen(filename, "r");
	if (yyin == NULL)
		log_error("could not open input file: %s", filename);

	/* push buffer state for lexer */
	yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));

	/* setup yytypes table for lexer */
	yytypes = hasht_new(8, true, NULL, NULL, &free_typename);
	log_assert(yytypes);

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

	log_debug("invoking Bison");
	int result = yyparse();
	if (result != 0)
		exit(2);

	/* print syntax tree */
	if (arguments.tree)
		tree_traverse(yyprogram, 0, &print_tree, NULL, NULL);

	/* initialize scope stack */
	log_debug("setting up for semantic analysis");
	yyscopes = list_new(NULL, NULL);
	log_assert(yyscopes);

	struct hasht *global = hasht_new(32, true, NULL, NULL, &symbol_free);
	log_assert(global);
	list_push_back(yyscopes, global);

	/* build the symbol tables */
	log_debug("populating symbol tables");
	region = GLOBAL_R;
	offset = 0;
	symbol_populate();
	log_debug("global scope had %zu symbols", hasht_used(global));

	/* setup constant region and offset */
	log_debug("type checking");

	/* constant symbol table put in front of stack for known location */
	struct hasht *constant = hasht_new(32, true, NULL, NULL, &symbol_free);
	log_assert(constant);
	list_push_front(yyscopes, constant);

	region = CONST_R;
	offset = 0;
	type_check(yyprogram);

	/* generating intermediate code */
	struct list *code = code_generate(yyprogram);
	print_code(stderr, code);

	/* clean up */
	log_debug("cleaning up");
	tree_free(yyprogram);
	yylex_destroy();
	hasht_free(yytypes);
	free(yyincludes); /* values all referenced elsewhere */
	list_free(yyfiles);
	list_free(yyscopes);
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
	case 'I':
		arguments->include = arg;
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
