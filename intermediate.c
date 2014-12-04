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
#include <stdio.h>

#include "intermediate.h"
#include "symbol.h"
#include "scope.h"

#include "logger.h"
#include "node.h"
#include "list.h"
#include "tree.h"

extern enum region region;
extern size_t offset;

extern struct typeinfo int_type;
extern struct typeinfo double_type;
extern struct typeinfo char_type;
extern struct typeinfo string_type;
extern struct typeinfo bool_type;
extern struct typeinfo void_type;
extern struct typeinfo class_type;
extern struct typeinfo unknown_type;

static struct op *op_new(enum opcode code, char *name,
                         struct address a, struct address b, struct address c);
static void push_op(struct node *n, struct op *op);
static struct op *label_new();
static struct address temp_new(struct typeinfo *t);
static struct list *get_code(struct tree *t, int i);
static struct address get_place(struct tree *t, int i);
static struct address get_label(struct op *op);

struct address e;

void code_generate(struct tree *t)
{
	struct node *n = t->data;
	/** pre-order **/
	/* TODO: manage scopes and memory regions */
	/* function invocation; public class member/function access; private
	   class member/function access when inside n-1 */
	size_t scopes = list_size(yyscopes);
	bool scoped = false;
	switch (n->rule) {
	case FUNCTION_DEF1:
	case FUNCTION_DEF2: {
		/* manage scopes for function recursion */
		scoped = true;
		char *k = get_identifier(t);
		struct typeinfo *f = scope_search(k);
		log_assert(f);
		log_debug("pushing function %s scope", k);
		scope_push(f->function.symbols);
		log_assert(list_size(yyscopes) == scopes + 1);
		break;
	}
	default: {
		break;
	}
	}

	/* recurse through children */
	struct list_node *iter = list_head(t->children);
	while (!list_end(iter)) {
		code_generate(iter->data);
		iter = iter->next;
	}

	/* leaf nodes have no code associated with them since all uses of
	   symbols are handled in higher nodes */
	if (tree_size(t) == 1) {
		/* this step should already be handled in type_check() */
		/* n->place = scope_search(n->token->text)->place; */
		log_assert(n->rule == TOKEN);
		log_assert(n->token);
		return;
	}

	/** post-order **/
	switch(n->rule) {
	}
	case INITIALIZER: {
		/* ASN(left, right, empty) */
		break;
	}
	case SELECT1: { /* IF */
		struct address temp = temp_new(&bool_type);
		struct op *first = label_new();
		struct op *follow = label_new();
		push_op(n, op_new(ASN, NULL, temp, get_place(t, 1), empty));
		push_op(n, op_new(BIF, NULL, temp, get_label(first), empty));
		/* TODO: backpatch this follow with parent's follow */
		push_op(n, op_new(GOTO, NULL, get_label(follow), empty, empty));
		break;
	}
	case SELECT2: { /* IF-ELSE chains */
		/* ASN(temp, condition) -> BIF(temp, first) -> GOTO(follow) ->
		   LABEL(first) -> get_code(n, 2) ->
		   LABEL(FOLLOW) -> get_code(n, 4) */
		struct address temp = temp_new(&bool_type);
		struct op *first = label_new();
		struct op *follow = label_new();
		push_op(n, op_new(ASN, NULL, temp, get_place(t, 1), empty));
		push_op(n, op_new(BIF, NULL, temp, get_label(first), empty));
		push_op(n, op_new(GOTO, NULL, get_label(follow), empty, empty));
		push_op(n, first);
		list_concat(n->code, get_code(t, 2));
		push_op(n, follow);
		list_concat(n->code, get_code(t, 4));
		break;
	}
	default: {
		break;
	}
	}
	/* concatenate all children code to build list */
	iter = list_head(t->children);
	while (!list_end(iter)) {
		struct tree *child = iter->data;
		struct node *n_ = child->data;
		/* noop for NULL code */
		n->code = list_concat(n->code, n_->code);
		iter = iter->next;
	}

	while (scoped && list_size(yyscopes) != scopes) {
		log_debug("popping scope");
		scope_pop();
	}
}

static struct op *op_new(enum opcode code, char *name,
                         struct address a, struct address b, struct address c)
{
	struct op *op = malloc(sizeof(*op));
	if (op == NULL)
		log_error("op_new(): could not malloc op");

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

/*
 * Return a new op with the LABEL pseudo code.
 *
 * Keeps track of the number of labels created. Labels are 0 indexed
 * in address region LABEL_R.
 */
static struct op *label_new()
{
	static size_t labels = 0;
	struct address place = { LABEL_R, labels };
	++labels;
	return op_new(LABEL, NULL, place, e, e);
}

/*
 * Given a type for size reference, return an address for a temporary
 * of the size of that type (incrementing the global offset).
 */
static struct address temp_new(struct typeinfo *t)
{
	struct address place = { region, offset };
	offset += typeinfo_size(t);
	return place;
}

static struct list *get_code(struct tree *t, int i)
{
	struct node *n = get_node(t, i);
	if (n)
		return n->code;
	else
		return NULL;
}

static struct address get_place(struct tree *t, int i)
{
	struct node *n = get_node(t, i);
	if (n)
		return n->place;
	else
		return e;
}

static struct address get_label(struct op *op)
{
	log_assert(op->code == LABEL);
	return op->address[0];
}

#define R(rule) case rule: return #rule
static char *print_opcode(enum opcode code)
{
	switch (code) {
		R(GLOBAL);
		R(PROC);
		R(LOCAL);
		R(LABEL);
		R(ADD);
		R(SUB);
		R(MUL);
		R(DIV);
		R(NEG);
		R(ASN);
		R(ADDR);
		R(LCONT);
		R(SCONT);
		R(GOTO);
		R(BLT);
		R(BLE);
		R(BGT);
		R(BGE);
		R(BEQ);
		R(BNE);
		R(BIF);
		R(BNIF);
		R(PARAM);
		R(CALL);
		R(RET);
	}
	return NULL;
}
#undef R

static void print_op(FILE *stream, struct op *op)
{
	fprintf(stream, "%s\n", print_opcode(op->code));
}

void print_code(FILE *stream, struct list *code)
{
	struct list_node *iter = list_head(code);
	while (!list_end(iter)) {
		if (iter->data)
			print_op(stream, iter->data);
		iter = iter->next;
	}
}
