/*
 * node.h - Semantic attributes for parse tree nodes.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef NODE_H
#define NODE_H

#include <stddef.h>

#include "rules.h"

struct tree;

/* TODO: narrow these down */
enum region {
	GLOBAL_R,
	LOCAL_R,
	PARAM_R,
	CLASS_R,
	LABEL_R,
	CONST_R,
	UNKNOWN_R
};

char *print_region(enum region r);

struct address {
	enum region region;
	size_t offset;
};

struct node {
	enum rule rule;
	struct address place;
	struct list *code;
	struct token *token;
};

struct node *node_new(enum rule r);
struct node *get_node(struct tree *t, size_t i);
enum rule get_rule(struct tree *t);
struct token *get_token(struct node *n);

#endif /* NODE_H */
