/*
 * intermediate.c -
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdlib.h>
#include <stddef.h>

#include "intermediate.h"
#include "logger.h"
#include "node.h"
#include "list.h"
#include "tree.h"

extern struct tree *yyprogram;

static void handle_node(struct tree *t, int d);
static struct op *op_new(enum opcode code, const char *name,
                         struct address a, struct address b, struct address c);
static void push_op(struct node *n, struct op *op);

void code_generate()
{
	tree_traverse(yyprogram, 0, NULL, NULL, &handle_node);
}

static void handle_node(struct tree *t, int d)
{
	struct node *n = t->data;
	switch(get_rule(t)) {
	case COMPOUND_STATEMENT:
		n->code = list_concat(n->code, get_node(t, 1)->code);
		break;
	case STATEMENT_SEQ1:
		n->code = list_concat(n->code, get_node(t, 0)->code);
		break;
	case STATEMENT_SEQ2:
		n->code = list_concat(get_node(t, 0)->code, get_node(t, 1)->code);
		n->code = list_concat(n->code, get_node(t, 0)->code);
		break;
	default:
		break;
	}
}

static struct op *op_new(enum opcode code, const char *name,
                         struct address a, struct address b, struct address c);
{
	struct op *op = malloc(sizeof(*op));
	log_assert(op);

	op->code = code;
	op->name = name;
	op->address[0] = a;
	op->address[1] = b;
	op->address[2] = c;

	return op;
}

static void push_op(struct node *n, struct op *op)
{
	if (n->code == NULL)
		n->code = list_new(NULL, NULL);
	log_assert(n->code);
	list_push_back(n->code, op);
}
