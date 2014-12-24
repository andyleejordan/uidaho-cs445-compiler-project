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
#include <libgen.h>

#include "args.h"
#include "logger.h"

#include "libs.h"
#include "lexer.h"
#include "symbol.h"
#include "node.h"
#include "scope.h"
#include "intermediate.h"
#include "final.h"

#include "list.h"
#include "tree.h"
#include "hasht.h"

/* argument parser */
const char *argp_program_version = "120++ hw5";
const char *argp_program_bug_address = "<andrew@schwartzmeyer.com>";

static char doc[] = "Andrew Schwartzmeyer's 120++ compiler.\v"

	"University of Idaho - Department of Computer Science\n"
	"CS 445: Compiler and Translator Design, by Dr. Clinton Jeffery"
	"\n\n"

	"This is a complete old-school compiler using Flex and Bison for a "
	"subset of C++ and C, dubbed '120++' as it should work for most code "
	"written in our CS 120 course, including basic classes. Notable "
	"exceptions include exceptions, templates, virtual semantics, "
	"namespaces, variadics, casting, conversions, typedefs, access and "
	"storage qualifiers, operator overloading, and assembly code "
	"generation. The final output is three-address C code, compilable by "
	"GCC, which demonstrates the correctness of the lexer, parser, "
	"semantic analysis, memory layout, and intermediate code generation. "
	"See the 'data/pass' folder for examples of valid code, and run "
	"`make smoke` to see their output."
	"\n\n"

	"This repo is located at: https://github.com/andschwa/uidaho-cs445"
	"\n\n"

	"This program is licensed under the AGPLv3, see the LICENSE file.\n";

static char args_doc[] = "infile...";

static struct argp_option options[] = {
	{ "debug",    'd', 0,      0, "Print debug messages (scopes, mostly).\n"
	  "Also disables exit on assertion failure."},
	{ "tree",     't', 0,      0, "Print the syntax tree." },
	{ "syntax",   't', 0,      OPTION_ALIAS },
	{ "symbols",  'y', 0,      0, "Print the populated symbols." },
	{ "checks",   'k', 0,      0, "Print the performed type checks." },
	{ "types",    'k', 0,      OPTION_ALIAS },
	{ "include",  'I', "DIR",  0, "Search path for 'system' headers." },
	{ "assemble", 's', 0,      0, "Generate assembler code." },
	{ "compile",  'c', 0,      0, "Generate object code." },
	{ "output",   'o', "FILE", 0, "Name of generated executable." },
	{ 0 }
};

static error_t parse_opt(int key, char *arg, struct argp_state *state);
static struct argp argp = { options, parse_opt, args_doc, doc };

/* shared with lexer and parser */
struct tree *yyprogram;
struct list *yyscopes;
struct list *yyfiles;
struct list *yyclibs;
struct hasht *yyincludes;
struct hasht *yytypes;
size_t yylabels;

enum region region;
size_t offset;

static void parse_program(char *filename);

/* from lexer */
void free_typename(struct hasht_node *t);

/* from parser */
int yyparse();
bool print_tree(struct tree *t, int d);

int main(int argc, char **argv)
{
	arguments.debug = false;
	arguments.tree = false;
	arguments.symbols = false;
	arguments.checks = false;
	arguments.assemble = false;
	arguments.compile = false;
	arguments.output = "a.out";
	arguments.include = getcwd(NULL, 0);

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	char *objects = "";
	/* parse each input file as a new 'program' */
	for (int i = 0; arguments.input_files[i]; ++i) {
		char *filename = realpath(arguments.input_files[i], NULL);
		if (filename == NULL)
			log_error("could not find input file: %s",
			          arguments.input_files[i]);
		/* copy because Wormulon's basename modifies */
		char *copy = strdup(filename);
		const char *base = basename(copy);
		free(copy);
		/* append object name to end of objects */
		char *temp;
		asprintf(&temp, "%s %s.o", objects, base);
		/* free if previously allocated */
		if (strcmp(objects, "") != 0)
			free(objects);
		objects = temp;
		parse_program(filename);
	}

	/* link object files */
	if (!arguments.assemble && !arguments.compile) {
		char *command;
		asprintf(&command, "gcc -o %s%s", arguments.output, objects);
		int status = system(command);
		if (status != 0)
			log_error("command failed: %s", command);
		free(command);
		/* remove object files */
		asprintf(&command, "rm -f%s", objects);
		status = system(command);
		if (status != 0)
			log_error("command failed: %s", command);
		free(command);
	}

	return EXIT_SUCCESS;
}

void parse_program(char *filename)
{
	printf("parsing file: %s\n", filename);

	yyfiles = list_new(NULL, &free);
	log_assert(yyfiles);
	list_push_back(yyfiles, filename);
	yyclibs = list_new(NULL, &free);
	log_assert(yyclibs);

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
	region = GLOBE_R;
	offset = 0;
	symbol_populate(yyprogram);
	log_debug("global scope had %zu symbols", hasht_used(global));

	/* constant symbol table put in front of stack for known location */
	struct hasht *constant = hasht_new(32, true, NULL, NULL, &symbol_free);
	log_assert(constant);
	list_push_front(yyscopes, constant);

	region = CONST_R;
	offset = 0;
	log_debug("type checking");
	type_check(yyprogram);

	/* generating intermediate code */
	log_debug("generating intermediate code");
	yylabels = 0; /* reset label counter */
	code_generate(yyprogram);
	struct list *code = ((struct node *)yyprogram->data)->code;

	/* iterate to get correct size of constant region */
	size_t string_size = 0;
	for (size_t i = 0; i < constant->size; ++i) {
		struct hasht_node *slot = constant->table[i];
		if (slot && !hasht_node_deleted(slot)) {
			struct typeinfo *v = slot->value;
			if (v->base == FLOAT_T)
				string_size += 8;
			else if (v->base == CHAR_T && v->pointer)
				string_size += v->token->ssize;
		}
	}

	/* print intermediate code file if debugging */
	if (arguments.debug) {
		char *output_file;
		asprintf(&output_file, "%s.ic", filename);
		FILE *ic = fopen(output_file, "w");
		if (ic == NULL)
			log_error("could not save to output file: %s",
			          output_file);

		fprintf(ic, ".file \"%s\"\n", filename);

		/* print .string region */
		fprintf(ic, ".string %zu\n", string_size);
		/* iterate to print everything but ints, chars, and bools */
		for (size_t i = 0; i < constant->size; ++i) {
			struct hasht_node *slot = constant->table[i];
			if (slot && !hasht_node_deleted(slot)) {
				struct typeinfo *v = slot->value;
				if (v->base == FLOAT_T
				    || (v->base == CHAR_T && v->pointer)) {
					fprintf(ic, "    ");
					print_typeinfo(ic, slot->key, v);
					fprintf(ic, "\n");
				}
			}
		}

		/* print .data region */
		fprintf(ic, ".data\n");
		for (size_t i = 0; i < global->size; ++i) {
			struct hasht_node *slot = global->table[i];
			if (slot && !hasht_node_deleted(slot)) {
				struct typeinfo *value = slot->value;
				if (value->base != FUNCTION_T) {
					fprintf(ic, "    ");
					print_typeinfo(ic, slot->key, value);
					fprintf(ic, "\n");
				}
			}
		}
		fprintf(ic, ".code\n");
		print_code(ic, code);
		fclose(ic);
		free(output_file);
	}

	log_debug("generating final code");
	/* copy because Wormulon */
	char *copy = strdup(filename);
	char *base = basename(copy);
	free(copy);
	char *output_file;
	asprintf(&output_file, "%s.c", base);
	FILE *fc = fopen(output_file, "w");
	if (fc == NULL)
		log_error("could not save to output file: %s", output_file);

	fprintf(fc, "/*\n");
	fprintf(fc, " * 120++ Generated Three Address C Code\n");
	fprintf(fc, " *\n");
	fprintf(fc, " * Copyright (C) 2014 Andrew Schwartzmeyer\n");
	fprintf(fc, " *\n");
	fprintf(fc, " */\n\n");

	/* setup necessary includes */
	fprintf(fc, "#include <stdlib.h>\n");
	fprintf(fc, "#include <stdbool.h>\n");
	fprintf(fc, "#include <string.h>\n");
	if (libs.usingstd && libs.iostream)
		fprintf(fc, "#include <stdio.h>\n");
	fprintf(fc, "\n");

	/* include passed-through C headers */
	fprintf(fc, "/* Source-file C headers */\n");
	struct list_node *iter = list_head(yyclibs);
	while (!list_end(iter)) {
		fprintf(fc, "#include %s\n", (char *)iter->data);
		iter = iter->next;
	}

	/* get maximum param size for faux stack */
	size_t max_param_size = 0;
	iter = list_head(code);
	while (!list_end(iter)) {
		struct op *op = iter->data;
		if (op->code == PROC_O) {
			size_t param_size = op->address[0].offset;
			if (param_size > max_param_size)
				max_param_size = param_size;
		}
		iter = iter->next;
	}

	/* setup constant, global, and stack regions' space */
	fprintf(fc, "char constant[%zu];\n", string_size);
	fprintf(fc, "char global[%zu];\n", global->size);
	fprintf(fc, "char stack[%zu];\n", max_param_size);
	fprintf(fc, "\n");

	/* generate final code instructions */
	final_code(fc, code);
	fclose(fc);

	/* remove TAC-C "assembler" code */
	if (!arguments.assemble) {
		/* compile object files */
		char *command;
		asprintf(&command, "gcc -c %s", output_file);
		int status = system(command);
		if (status != 0)
			log_error("command failed: %s", command);

		remove(output_file);
	}

	free(output_file);

	/* clean up */
	log_debug("cleaning up");
	tree_free(yyprogram);
	yylex_destroy();
	hasht_free(yytypes);
	free(yyincludes); /* values all referenced elsewhere */
	list_free(yyfiles);
	list_free(yyclibs);
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
	case 'y':
		arguments->symbols = true;
		break;
	case 'k':
		arguments->checks = true;
		break;
	case 'I':
		arguments->include = arg;
		break;
	case 's':
		arguments->assemble = true;
		break;
	case 'c':
		arguments->compile = true;
		break;
	case 'o':
		arguments->output = arg;
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
