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

/* TODO: narrow these down */
enum region {
        GLOBAL_R,
        LOCAL_R,
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
	int rule;
	struct address place;
	struct token *token;
};

struct node *node_new(int r);

#endif /* NODE_H */
