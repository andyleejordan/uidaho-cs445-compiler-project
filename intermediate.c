/*
 * intermediate.c -
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stddef.h>

#include "node.h"
#include "tree.h"

extern struct tree *yyprogram;

static enum region region;
static size_t offset;
static char *print_region(enum region r);

void place_populate()
{
	offset = 0;
	region = GLOBAL_R;

	/* do a top-down pre-order traversal to populate symbol tables */
	/* tree_preorder(yyprogram, 0, &handle_node); */
}


/*
 * Given a region enu, returns its name as a static string.
 */
#define R(rule) case rule: return #rule
static char *print_region(enum region r)
{
	switch (r) {
		R(GLOBAL_R);
		R(LOCAL_R);
		R(CLASS_R);
		R(LABEL_R);
		R(CONST_R);
		R(UNKNOWN_R);
	};

	return NULL; /* error */
}
#undef R
