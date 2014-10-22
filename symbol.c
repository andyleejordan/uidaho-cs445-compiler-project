/*
 * symbol.c - Implementation of symbol data.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "symbol.h"
#include "token.h"
#include "list.h"
#include "hasht.h"
#include "tree.h"
#include "rules.h"
#include "lexer.h"
#include "parser.tab.h"

struct list *yyscopes;

#define current_scope() (struct hasht *)list_back(yyscopes)
#define push_scope(k) list_push_back(yyscopes, get_scope(current_scope(), k))
#define pop_scope() list_pop_back(yyscopes)

#define get_rule(n) *(enum rule *)n->data
#define get_token(n, i) ((struct token *)tree_index(n, i)->data)

/* from lexer */
extern struct hasht *typenames;
extern bool usingstd;
extern bool fstream;
extern bool iostream;
extern bool string;

#define R(rule) case rule: return #rule

char *print_type(enum type t)
{
	switch (t) {
		R(INT_T);
		R(DOUBLE_T);
		R(CHAR_T);
		R(BOOL_T);
		R(ARRAY_T);
		R(FUNCTION_T);
		R(CLASS_T);
		R(VOID_T);
		R(UNKNOWN_T);
	}
}

#undef R

void semantic_error(char *s, struct tree *n)
{
	while (tree_size(n) > 1)
		n = list_front(n->children);
	struct token *t = n->data;
	fprintf(stderr, "Semantic error: file %s, line %d, token %s: %s\n",
	        t->filename, t->lineno, t->text, s);
	exit(3);
}

void *find_declared(char *k)
{
	struct list_node *iter = list_tail(yyscopes);
	while (!list_end(iter)) {
		struct typeinfo *t = hasht_search(iter->data, k);
		if (t)
			return t;
		iter = iter->prev;
	}
	return NULL;
}

bool prototype_compare(struct typeinfo *a, struct typeinfo *b);

void insert_symbol(char *k, struct typeinfo *v, struct tree *n, struct hasht *l)
{
	struct typeinfo *e = find_declared(k);
	if (e == NULL) {
		fprintf(stderr, "inserting ident %s into table %zu\n", k, list_size(yyscopes));
		hasht_insert(current_scope(), k, v);
	} else if (e->base == FUNCTION_T && v->base == FUNCTION_T) {
		if (!prototype_compare(e, v)) {
			semantic_error("function prototypes mismatched", n);
		} else if (l) {
			e->function.symbols = l;
		}
	} else if (e->base == CLASS_T && v->base == CLASS_T) {
		fprintf(stderr, "classes not implemented yet\n");
	} else {
		semantic_error("identifier already declared", n);
	}
}

enum type get_type(enum yytokentype t)
{
	switch (t) {
	case INT:
	case SHORT:
	case LONG:
		return INT_T;
	case FLOAT:
	case DOUBLE:
		return DOUBLE_T;
	case CHAR:
		return CHAR_T;
	case BOOL:
		return BOOL_T;
	case CLASS:
	case STRUCT:
		return CLASS_T;
	case VOID:
		return VOID_T;
	default:
		return UNKNOWN_T;
	}
}

/*
 * Given a scope and key, get the nested scope for the key.
 */
struct hasht *get_scope(struct hasht *s, char *k)
{
	struct typeinfo *t = hasht_search(s, k);
	if (t == NULL)
		return NULL;

	switch (t->base) {
	case FUNCTION_T:
		return t->function.symbols;
	case CLASS_T:
		return t->class.symbols;
	default:
		return NULL; /* error */
	}
}

/*
 * Constructs new typeinfo.
 *
 * array: count 2, typeinfo, size
 * function: count 3, typeinfo, parameters list, scope
 * class: count 2, name, scope
 */
struct typeinfo *typeinfo_new(enum type base, bool pointer, int count, ...)
{
	va_list ap;
	va_start(ap, count);

	struct typeinfo *t = malloc(sizeof(*t));
	t->base = base;
	t->pointer = pointer;

	switch (t->base) {
	case ARRAY_T: {
		t->array.type = va_arg(ap, struct typeinfo *);
		t->array.size = va_arg(ap, size_t);
		break;
	}
	case FUNCTION_T: {
		t->function.type = va_arg(ap, struct typeinfo *);
		t->function.parameters = va_arg(ap, struct list *);
		t->function.symbols = va_arg(ap, struct hasht *);
		break;
	}
	case CLASS_T: {
		t->class.type = va_arg(ap, char *);
		t->class.symbols = va_arg(ap, struct hasht *);
		break;
	}
	default:
		break;
	}

	va_end(ap);
	return t;
}

/*
 * Frees types for array and function.
 */
void typeinfo_delete(struct typeinfo *t)
{
	switch (t->base) {
	case ARRAY_T: {
		typeinfo_delete(t->array.type);
		break;
	}
	case FUNCTION_T: {
		list_free(t->function.parameters);
		typeinfo_delete(t->function.type);
		break;
	}
	case CLASS_T: {
		break;
	}
	default:
		break;
	}
}

bool typeinfo_compare(struct typeinfo *a, struct typeinfo *b)
{
	if (a == NULL && b == NULL)
		return true;

	if (a == NULL || b == NULL)
		return false;

	if (a->base != b->base)
		return false;

	if (a->pointer != b->pointer)
		return false;

	switch (a->base) {
	case ARRAY_T: {
		if (!typeinfo_compare(a->array.type, b->array.type))
			return false;

		if (a->array.size != b->array.size)
			return false;

		return true;
	}
	case FUNCTION_T: {
		if (!typeinfo_compare(a->function.type, b->function.type))
			return false;

		if (!prototype_compare(a, b))
			return false;

		return true;
	}
	case CLASS_T: {
		return strcmp(a->class.type, b->class.type) == 0;
	}
	case UNKNOWN_T:
		return false;
	default:
		return true;
	}
}

bool prototype_compare(struct typeinfo *a, struct typeinfo *b)
{
	struct list_node *a_iter = list_head(a->function.parameters);
	struct list_node *b_iter = list_head(b->function.parameters);

	while (!list_end(a_iter) && !list_end(b_iter)) {
		if (!typeinfo_compare(a_iter->data, b_iter->data))
			return false;

		a_iter = a_iter->next;
		b_iter = b_iter->next;
	}
	return list_end(a_iter) && list_end(b_iter);
}

/*
 * Handles a PARAM_DECL1 rule for basic types, pointers, and arrays
 */
void handle_param(struct hasht *s, struct tree *n)
{
	char *k = NULL;
	struct typeinfo *v = NULL;
	enum type t = get_type(get_token(n, 0)->category);
	switch (tree_size(n)) {
	case 3: { /* simple parameter */
		k = get_token(n, 1)->text;
		v = typeinfo_new(t, false, 0);
		break;
	}
	case 5: { /* simple parameter pointer */
		k = get_token(tree_index(n, 1), 1)->text;
		v = typeinfo_new(t, true, 0);
		break;
	}
	case 6: { /* simple array parameter */
		k = get_token(tree_index(n, 1), 0)->text;
		v = typeinfo_new(ARRAY_T, false, 2, typeinfo_new(t, false, 0), 0);
		break;
	}
	default:
		semantic_error("parameter declaration", n);
	}
	hasht_insert(s, k, v);
}

/*
 * Handles a PARAM_DECL1 or a PARAM_DECL3 rule
 */
void proto_param(struct list *p, struct tree *n)
{
	struct typeinfo *v = NULL;
	enum type t = get_type(get_token(n, 0)->category);

	if (tree_size(n) == 2) { /* simple variable */
		v = typeinfo_new(t, false, 0);
	} else {
		switch (get_rule(tree_index(n, 1))) {
		case ABSTRACT_DECL1:
		case DECL2: {
			v = typeinfo_new(t, true, 0);
			break;
		}
		case DIRECT_ABSTRACT_DECL4:
		case DIRECT_DECL6: {
			v = typeinfo_new(ARRAY_T, false, 2, typeinfo_new(t, false, 0), 0);
			break;
		}
		default: {
			if (tree_size(n) == 3)
				v = typeinfo_new(t, false, 0);
			else
				semantic_error("prototype unsupported", n);
		}
		}
	}
	list_push_back(p, v);
}

/*
 * Handles an arbitrarily nested list of parameters recursively
 */
void handle_param_list(struct hasht *local, struct tree *param)
{
	if (get_rule(param) == PARAM_DECL1) {
		handle_param(local, param);
	} else {
		struct list_node *iter = list_head(param->children);
		while (!list_end(iter)) {
			handle_param_list(local, iter->data);
			iter = iter->next;
		}
	}
}

void proto_param_list(struct list *p, struct tree *param)
{
	if (get_rule(param) == PARAM_DECL1 || get_rule(param) == PARAM_DECL3) {
		proto_param(p, param);
	} else {
		struct list_node *iter = list_head(param->children);
		while (!list_end(iter)) {
			proto_param_list(p, iter->data);
			iter = iter->next;
		}
	}
}

bool handle_node(struct tree *n, int d)
{
	switch (get_rule(n)) {
	case SIMPLE_DECL1: {
		/* this may need to be synthesized */
		enum type t = get_type(get_token(n, 0)->category);

		char *k = NULL;
		struct typeinfo *v = NULL;

		struct tree *init_decl = tree_index(n, 1);
		struct tree *direct_decl = tree_index(init_decl, 0);

		switch (get_rule(direct_decl)) {
		case DECL2: { /* simple pointer variable */
			k = get_token(direct_decl, 1)->text;
			v = typeinfo_new(t, true, 0);
			break;
		}
		case DIRECT_DECL6: { /* simple array with size */
			k = get_token(direct_decl, 0)->text;
			size_t size = get_token(direct_decl, 2)->ival;
			v = typeinfo_new(ARRAY_T, false, 2, size, t);
			break;
		}
		case DIRECT_DECL2: { /* function prototype */
			k = get_token(direct_decl, 0)->text;
			struct list *params = list_new(NULL, NULL);
			if (tree_size(direct_decl) > 2)
				proto_param_list(params, tree_index(direct_decl, 1));
			v = typeinfo_new(FUNCTION_T, false, 3, t, params, NULL);
			break;
		}
		default: {
			/* simple variable, possibly with initializer */
			if (tree_size(init_decl) <= 3) {
				k = get_token(init_decl, 0)->text;
				v = typeinfo_new(t, false, 0);
			} else {
				semantic_error("unsupported declaration type", n);
			}
		}
		}
		if (v)
			insert_symbol(k, v, n, NULL);
		return false;
	}
	case FUNCTION_DEF2: {
		struct hasht *local = hasht_new(32, true, NULL, NULL, &free_symbols);

		enum type t = get_type(get_token(n, 0)->category);
		struct tree *direct_decl = tree_index(n, 1);

		char *k = get_token(direct_decl, 0)->text;
		struct list *params = list_new(NULL, NULL);

		/* add parameters (if they exist) to local symbol table */
		if (tree_size(direct_decl) > 2) {
			proto_param_list(params, tree_index(direct_decl, 1));
			handle_param_list(local, tree_index(direct_decl, 1));
		}

		struct typeinfo *v = typeinfo_new(FUNCTION_T, false, 3, t, params, local);
		insert_symbol(k, v, n, local);

		/* recurse on children while in subscope */
		list_push_back(yyscopes, local);
		tree_preorder(tree_index(n, 2), d, &handle_node);
		pop_scope();

		return false;
	}
	default: /* rule did not provide a symbol, so recurse on children */
		return true;
	}
}


struct hasht *build_symbols(struct tree *syntax)
{
	struct hasht *global = hasht_new(32, true, NULL, NULL, &free_symbols);

	/* initialize scope stack */
	yyscopes = list_new(NULL, NULL);
	list_push_back(yyscopes, global);

	/* handle standard libraries */
	if (usingstd) {
		if (fstream) {
			hasht_insert(global, "ifstream",
			             typeinfo_new(CLASS_T, 2, false,
			                          hasht_search(typenames, "ifstream"), NULL));
			hasht_insert(global, "ofstream",
			             typeinfo_new(CLASS_T, 2, false,
			                          hasht_search(typenames, "ifstream"), NULL));
		}
		if (iostream) {
			hasht_insert(global, "cin",
			             typeinfo_new(CLASS_T, 2, false, "istream", NULL));
			hasht_insert(global, "cout",
			             typeinfo_new(CLASS_T, 2, false, "istream", NULL));
			hasht_insert(global, "endl",
			             typeinfo_new(CLASS_T, 2, false, "istream", NULL));
		}
		if (string) {
			hasht_insert(global, "string",
			             typeinfo_new(CLASS_T, 2, false,
			                          hasht_search(typenames, "string"), NULL));
		}
	}

	/* do a top-down pre-order traversal to populate symbol tables */
	tree_preorder(syntax, 0, &handle_node);

	list_free(yyscopes);

	return global;
}

void free_symbols(struct hash_node *n)
{
	free(n->key);
	typeinfo_delete(n->value);
}
