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
#include "type.h"
#include "symbol.h"
#include "scope.h"
#include "token.h"

#include "logger.h"
#include "node.h"
#include "list.h"
#include "tree.h"

extern size_t yylabels;
extern struct typeinfo int_type;
extern struct typeinfo float_type;
extern struct typeinfo char_type;
extern struct typeinfo string_type;
extern struct typeinfo bool_type;
extern struct typeinfo void_type;
extern struct typeinfo class_type;
extern struct typeinfo unknown_type;

static enum opcode map_code(enum rule r);
static struct op *op_new(enum opcode code, char *name,
                         struct address a, struct address b, struct address c);
static void push_op(struct node *n, struct op *op);
static struct op *label_new();
static struct address temp_new(struct typeinfo *t);
static struct list *get_code(struct tree *t, int i);
static struct address get_place(struct tree *t, int i);
static struct address get_label(struct op *op);

const struct address e = { UNKNOWN_R, 0, &unknown_type };
const struct address one = { CONST_R, 1, &int_type };

/*
 * Tree traversal to generate a list of three-address code
 * instructions given a parse tree. Handles scopes in pre-order,
 * instructions in post-order.
 */
#define append_code(i) do { n->code = list_concat(n->code, get_code(t, i)); } while (0)
void code_generate(struct tree *t)
{
	struct node *n = t->data;
	/** pre-order **/
	/* function invocation; public class member/function access; private
	   class member/function access when inside n-1 */
	size_t scopes = list_size(yyscopes);
	bool scoped = false;
	enum region region_ = region;
	size_t offset_ = offset;

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
		/* manage memory regions */
		region = LOCAL_R;
		offset = scope_size(f->function.symbols);

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
	if (n->rule == TOKEN) {
		/* this step should already be handled in type_check() */
		/* n->place = scope_search(n->token->text)->place; */
		log_assert(n->token);
		return;
	}

	/** post-order **/
	switch(n->rule) {
	case INITIALIZER:
	case LITERAL: {
		/* this passes up place for symbols, but get_address()
		   may make it unnecessary */
		/* TODO: get ident, search, get address */
		n->place = get_place(t, 0);
		append_code(0);
		break;
	}
	case INIT_DECL: {
		n->place = get_place(t, 0);

		struct node *init = get_node(t, 1);
		if (init == NULL)
			break;
		append_code(1);
		push_op(n, op_new(ASN, get_identifier(t), n->place, init->place, e));
		break;
	}
	case POSTFIX_EXPR3: {
		char *k = get_identifier(t);
		n->place = temp_new(scope_search(k));
		/* count number of parameters */
		struct address count;
		count.region = CONST_R;
		count.type = &int_type;
		count.offset = list_size(scope_search(k)->function.parameters);
		append_code(1); /* parameters */
		push_op(n, op_new(CALL, k, n->place, count, e));
		break;
	}
	case EXPR_LIST: {
		iter = list_head(t->children);
		while (!list_end(iter)) {
			struct tree *child = iter->data;
			struct node *n_ = child->data;
			n->code = list_concat(n->code, n_->code);
			struct address place = get_place(child, -1);
			if (place.region != UNKNOWN_R)
				push_op(n, op_new(PARAM, NULL, place, e, e));
			iter = iter->next;
		}
		break;
	}
	case SELECT1: { /* IF */
		struct op *first = label_new();
		struct op *follow = label_new();
		append_code(1); /* condition */
		push_op(n, op_new(BIF, NULL, get_place(t, 1), get_label(first), e));
		/* TODO: backpatch this follow with parent's follow */
		push_op(n, op_new(GOTO, NULL, get_label(follow), e, e));
		push_op(n, first);
		append_code(2); /* true */
		push_op(n, follow);
		break;
	}
	case SELECT2: { /* IF-ELSE chains */
		/* ASN(temp, condition) -> BIF(temp, first) -> GOTO(follow) ->
		   LABEL(first) -> get_code(n, 2) ->
		   LABEL(FOLLOW) -> get_code(n, 4) */
		struct op *first = label_new();
		struct op *follow = label_new();
		append_code(1); /* condition */
		push_op(n, op_new(BIF, NULL, get_place(t, 1), get_label(first), e));
		push_op(n, op_new(GOTO, NULL, get_label(follow), e, e));
		push_op(n, first);
		append_code(2); /* true */
		push_op(n, follow);
		append_code(4); /* false */
		break;
	}
	case ITER1: { /* while (expr) { body; } */
		struct op *first = label_new();
		struct op *body = label_new();
		struct op *follow = label_new();
		push_op(n, first); /* before condition */
		append_code(1); /* expr */
		push_op(n, op_new(BIF, NULL, get_place(t, 1), get_label(body), e));
		push_op(n, op_new(GOTO, NULL, get_label(follow), e, e));
		push_op(n, body);
		append_code(2); /* body */
		push_op(n, op_new(GOTO, NULL, get_label(first), e, e));
		push_op(n, follow);
		break;
	}
	case ITER2: { /* do { body; } while (expr); */
		struct op *first = label_new();
		push_op(n, first); /* before body */
		append_code(1); /* body */
		append_code(3); /* expr */
		push_op(n, op_new(BIF, NULL, get_place(t, 3), get_label(first), e));
		break;
	}
	case ITER3: { /* for (expr1; expr2; expr3) { body; } */
		append_code(1); /* expr 1 */
		struct op *first = label_new();
		struct op *body = label_new();
		struct op *follow = label_new();
		push_op(n, first); /* before condition */
		append_code(2); /* expr 2 */
		push_op(n, op_new(BIF, NULL, get_place(t, 2), get_label(body), e));
		push_op(n, op_new(GOTO, NULL, get_label(follow), e, e));
		push_op(n, body);
		append_code(4); /* body */
		append_code(3); /* expr3 */
		push_op(n, op_new(GOTO, NULL, get_label(first), e, e));
		push_op(n, follow);
		break;
	}
	case REL_EXPR2:
	case REL_EXPR3:
	case REL_EXPR4:
	case REL_EXPR5:
	case EQUAL_EXPR3:
	case EQUAL_EXPR2: {
		/* TODO: handle short circuiting */
		n->place = temp_new(&bool_type);
		struct address l = get_place(t, 0);
		append_code(0); /* left */
		struct address r = get_place(t, 2);
		append_code(2); /* right */
		push_op(n, op_new(map_code(n->rule), NULL, n->place, l, r));
		break;
	}
	case ADD_EXPR2:
	case ADD_EXPR3:
	case MULT_EXPR2:
	case MULT_EXPR3:
	case MULT_EXPR4: {
		n->place = temp_new(&int_type);
		struct address l = get_place(t, 0);
		append_code(0); /* left */
		struct address r = get_place(t, 2);
		append_code(2); /* right */
		push_op(n, op_new(map_code(n->rule), NULL, n->place, l, r));
		break;
	}
	case UNARY_EXPR3:
	case UNARY_EXPR2: {
		n->place = get_place(t, 1);
		push_op(n, op_new(map_code(n->rule), NULL, n->place, one, e));
		break;
	}
	case UNARY_EXPR7:
	case UNARY_EXPR8: { /* sizeof(symbol) */
		/* obtain size as a const int */
		struct address size;
		size.region = CONST_R;
		size.type = &int_type;

		struct typeinfo *type = NULL;

		char *k = get_identifier(t);
		if (k) {
			/* perform symbol lookup if possible */
			type = typeinfo_copy(scope_search(k));
			/* if dereferencing, use size of value */
			if (get_pointer(t))
				type->pointer = false;
			/* if accessing an index, use size of array element */
			if (get_array(t) > 0) {
				struct typeinfo *temp = typeinfo_copy(type->array.type);
				free(type);
				type = temp;
			}
		} else {
			/* get type specifier */
			struct tree *type_spec = get_production(t, TYPE_SPEC_SEQ);
			if (type_spec == NULL)
				log_semantic(t, "sizeof operator missing type spec");
			type = typeinfo_copy(type_check(tree_index(type_spec, 0)));
			/* check if given a pointer type */
			if (get_pointer(t))
				type->pointer = true;
		}
		if (type == NULL) /* still a semantic error */
			log_semantic(t, "sizeof operator missing type");

		size.offset = typeinfo_size(type);
		free(type);
		n->place = size;
		break;
	}
	case POSTFIX_EXPR2: { /* array[index] */
		char *k = get_identifier(t);
		struct typeinfo *array = scope_search(k);
		struct address index = get_place(t, 2);
		n->place = temp_new(array->array.type);
		push_op(n, op_new(ARR, NULL, n->place, array->place, index));
		break;
	}
	case POSTFIX_EXPR9:
	case POSTFIX_EXPR10: {
		n->place = get_place(t, 0);
		push_op(n, op_new(map_code(n->rule), NULL, n->place, one, e));
		break;
	}
	case ASSIGN_EXPR2: {
		char *k = get_identifier(t);
		n->place = get_place(t, 0);
		struct address r = get_place(t, 2);
		append_code(0); /* left */
		append_code(2); /* right */
		push_op(n, op_new(ASN, k, n->place, r, e));
		break;
	}
	case RETURN_STATEMENT: {
		struct node *ret = get_node(t, 1);
		if (ret == NULL)
			break;
		n->place = ret->place;
		push_op(n, op_new(RET, NULL, n->place, e, e));
		append_code(1);
		break;
	}
	case FUNCTION_DEF2: {
		/* TODO: get procedure parameter and local sizes */
		push_op(n, label_new());
		push_op(n, op_new(PROC, get_identifier(t), e, e, e));
		append_code(2);
		push_op(n, op_new(END, NULL, e, e, e));
		break;
	}
	default: {
		/* concatenate all children code to build list */
		iter = list_head(t->children);
		while (!list_end(iter)) {
			struct tree *child = iter->data;
			struct node *n_ = child->data;
			/* noop for NULL code */
			n->code = list_concat(n->code, n_->code);
			iter = iter->next;
		}
	}
	}

	if (scoped) {
		while (list_size(yyscopes) != scopes) {
			log_debug("popping scope");
			scope_pop();
		}

		/* restore region and offset */
		region = region_;
		offset = offset_;
	}
}
#undef append_code

/*
 * Maps a production rule to an opcode if supported.
 */
static enum opcode map_code(enum rule r)
{
	switch (r) {
	case REL_EXPR2:
		return BLT; /* < */
	case REL_EXPR3:
		return BGT; /* > */
	case REL_EXPR4:
		return BLE; /* <= */
	case REL_EXPR5:
		return BGE; /* >= */
	case EQUAL_EXPR2:
		return BEQ; /* == */
	case EQUAL_EXPR3:
		return BNE; /* != */
	case ADD_EXPR2:
	case UNARY_EXPR2:
	case POSTFIX_EXPR9:
		return ADD; /* + */
	case ADD_EXPR3:
	case UNARY_EXPR3:
	case POSTFIX_EXPR10:
		return SUB; /* - */
	case MULT_EXPR2:
		return MUL; /* * */
	case MULT_EXPR3:
		return DIV; /* / */
	case MULT_EXPR4:
		return MOD; /* % */
	default:
		return ERRC; /* unknown */
	}
}

/*
 * Allocates a new op, assigns code, name, and 3 addresses. Pass 'e'
 * address for 'empty' address slots.
 */
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

/*
 * Allocates a new code list on a node if necessary. Pushes op to back
 * of list.
 */
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
	struct address place = { LABEL_R, yylabels, &unknown_type };
	++yylabels;
	return op_new(LABEL, NULL, place, e, e);
}

/*
 * Given a type for size reference, return an address for a temporary
 * of the size of that type (incrementing the global offset).
 */
static struct address temp_new(struct typeinfo *t)
{
	struct address place = { region, offset, t };
	offset += typeinfo_size(t);
	return place;
}

/*
 * Returns code list given a child index on a tree if available.
 */
static struct list *get_code(struct tree *t, int i)
{
	struct node *n = get_node(t, i);

	if (n)
		return n->code;

	return NULL;
}

/*
 * Returns address given a child index on a tree. If negative, returns
 * own address. Looks up address based on token->text in scope if not
 * stored in the node.
 */
static struct address get_place(struct tree *t, int i)
{
	struct node *n = i > -1 ? get_node(t, i) : t->data;

	if (n) {
		if (n->place.region != UNKNOWN_R)
			return n->place;

		/* perform lookup if necessary */
		if (n->rule == TOKEN) {
			struct typeinfo *s = scope_search(n->token->text);
			if (s && s->place.region != UNKNOWN_R)
				return s->place;
		}
	}

	return e;
}

/*
 * Returns the label of an op (first address).
 */
static struct address get_label(struct op *op)
{
	log_assert(op->code == LABEL);
	return op->address[0];
}

/*
 * Returns static string for opcode.
 */
#define R(rule) case rule: return #rule
static char *print_opcode(enum opcode code)
{
	switch (code) {
		R(GLOBAL);
		R(PROC);
		R(LOCAL);
		R(LABEL);
		R(END);
		R(ADD);
		R(SUB);
		R(MUL);
		R(DIV);
		R(MOD);
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
		R(ARR);
		R(PARAM);
		R(CALL);
		R(RET);
		R(ERRC);
	}
	return NULL;
}
#undef R

/*
 * Given an op, prints it and its memory addresses.
 */
static void print_op(FILE *stream, struct op *op)
{
	fprintf(stream, "%-8s", print_opcode(op->code));
	fprintf(stream, "%-10s", op->name ? op->name : "");
	for (int i = 0; i < 3; ++i) {
		struct address a = op->address[i];
		if (a.region != UNKNOWN_R) {
			print_address(stream, a);
			fprintf(stream, " ");
		}
	}
	fprintf(stream, "\n");
}

/*
 * Given a stream and linked list of ops, prints each in order.
 */
void print_code(FILE *stream, struct list *code)
{
	struct list_node *iter = list_head(code);
	while (!list_end(iter)) {
		if (iter->data)
			print_op(stream, iter->data);
		iter = iter->next;
	}
}
