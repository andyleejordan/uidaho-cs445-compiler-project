/*
 * node.c - Implementation for nodes.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "node.h"
#include "logger.h"
#include "list.h"
#include "tree.h"
#include "rules.h"

/*
 * Allocates a new blank node.
 *
 * Nodes hold semantic attributes, such as production rule, memory
 * address (place field), and non-NULL token pointers if a leaf.
 *
 * TODO: add first/follow/true/false attributes
 */
struct node *node_new(enum rule r)
{
	struct node *n = malloc(sizeof(*n));
	if (n == NULL)
		log_error("could not allocate memory for node");

	n->rule = r;
	n->place.region = UNKNOWN_R;
	n->place.offset = 0;
	n->code = NULL;
	n->token = NULL;

	return n;
}

/*
 * Given a tree and position, returns subtree's node.
 *
 * Returns NULL if child tree not found.
 */
struct node *get_node(struct tree *t, size_t i)
{
	log_assert(t);

	if (tree_size(t) > 1)
		t = tree_index(t, i);

	if (t == NULL)
		return NULL;

	struct node *n = t->data;
	log_assert(n);

	return n;
}

/*
 * Given a tree node, extract its production rule.
 */
enum rule get_rule(struct tree *t)
{
	log_assert(t);

	struct node *n = t->data;
	return n->rule;
}

/*
 * Given a leaf node, extract its token, checking for NULL.
 */
struct token *get_token(struct node *n)
{
	log_assert(n);
	struct token *t = n->token;
	log_assert(t);
	return t;
}
