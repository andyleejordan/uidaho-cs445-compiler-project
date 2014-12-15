/*
 * args.h - Args available throughout program.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include <argp.h>
#include <stdbool.h>

struct arguments {
	bool debug;
	bool tree;
	bool symbols;
	bool checks;
	bool assemble;
	bool compile;
	char *output;
	char *include;
	char **input_files;
} arguments;

#endif /* ARGS_H */
