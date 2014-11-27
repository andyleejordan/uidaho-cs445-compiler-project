/*
 * node.c - Implementation for nodes.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdlib.h>

#include "node.h"
#include "logger.h"

/*
 * Given a region enum, returns its name as a static string.
 */
#define R(rule) case rule: return #rule
char *print_region(enum region r)
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

/*
 * Allocates a new blank node.
 *
 * Nodes hold semantic attributes, such as produciton rule, memory
 * address (.place field), and non-NULL token pointers if a leaf.
 */
struct node *node_new(int r)
{
	struct node *n = malloc(sizeof(*n));
	if (n == NULL)
		log_error("could not allocate memory for node");

	n->rule = r;
	n->place.region = UNKNOWN_R;
	n->place.offset = 0;
	n->token = NULL;

	return n;
}
