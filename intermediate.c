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
#include "list.h"
#include "tree.h"

extern struct tree *yyprogram;

static void handle_node(struct tree *t, int d);

void code_generate()
{
	tree_traverse(yyprogram, 0, NULL, NULL, &handle_node);
}

static void handle_node(struct tree *t, int d)
{
	struct node *n = t->data;
	switch(get_rule(t)) {
	case COMPOUND_STATEMENT:
		list_concat(n->code, get_node(t, 1)->code);
		break;
	case STATEMENT_SEQ1:
		list_concat(n->code, get_node(t, 0)->code);
		break;
	case STATEMENT_SEQ2:
		list_concat(get_node(t, 0)->code, get_node(t, 1)->code);
		list_concat(n->code, get_node(t, 0)->code);
		break;
	default:
		break;
	}
}
