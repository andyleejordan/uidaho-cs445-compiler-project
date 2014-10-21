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

#define current_scope() (struct hasht *)list_back(yyscopes)
#define push_scope(k) list_push_back(yyscopes, get_scope(current_scope(), k))
#define pop_scope() list_pop_back(yyscopes)

#define get_rule(n) *(enum rule *)n->data
#define get_token(n, i) ((struct token *)tree_index(n, i)->data)

#define insert_symbol(k, v) hasht_insert(current_scope(), k, v)

/* from lexer */
extern struct hasht *typenames;
extern bool usingstd;
extern bool fstream;
extern bool iostream;
extern bool string;

void semantic_error(char *s, struct tree *n)
{
	while (tree_size(n) > 1)
		n = list_front(n->children);
	struct token *t = n->data;
	fprintf(stderr, "Semantic error: file %s, line %d, token %s: %s\n",
	        t->filename, t->lineno, t->text, s);
	exit(3);
}	

bool check_declared(struct list *s, char *k)
{
	struct list_node *iter = list_tail(s);
	while (!list_end(iter)) {
		if (hasht_search(iter->data, k))
			return true;
		iter = iter->prev;
	}
	return false;
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
 * array: count 2, size, typeinfo
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
		t->array.size = va_arg(ap, size_t);
		t->array.type = va_arg(ap, struct typeinfo *);
	}
	case FUNCTION_T: {
		t->function.type = va_arg(ap, struct typeinfo *);
		t->function.parameters = va_arg(ap, struct list *);
		t->function.symbols = va_arg(ap, struct hasht *);
	}
	case CLASS_T: {
		t->class.type = va_arg(ap, char *);
		t->class.symbols = va_arg(ap, struct hasht *);
	}
	default:
		break;
	}

	va_end(ap);
		
	return t;
}	

void typeinfo_delete(struct typeinfo *t)
{
	switch (t->base) {
	case ARRAY_T:
		typeinfo_delete(t->array.type);
	case FUNCTION_T:
		list_free(t->function.parameters);
	case CLASS_T:
		free(t->class.type);
	default:
		break;
	}
}

struct hasht *build_symbols(struct tree *syntax)
{
	struct hasht *global = hasht_new(32, true, NULL, NULL, &free_symbols);

	/* initialize scope stack */
	struct list *yyscopes = list_new(NULL, NULL);
	list_push_back(yyscopes, global);

	/* handle standard libraries */
	if (usingstd) {
		if (fstream) {
			hasht_insert(global, "ifstream",
			             typeinfo_new(CLASS_T, 2, false, hasht_search(typenames, "ifstream"), NULL));
			hasht_insert(global, "ofstream",
			             typeinfo_new(CLASS_T, 2, false, hasht_search(typenames, "ifstream"), NULL));
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
			             typeinfo_new(CLASS_T, 2, false, hasht_search(typenames, "string"), NULL));
		}
	}

	/* breadth first traversal of syntax tree */
	struct list *queue = list_new(NULL, NULL);
	list_push_back(queue, syntax);
	while (!list_empty(queue)) {
		/* visit node */
		struct tree *n = list_pop_front(queue);
		
		switch (get_rule(n)) {
		case SIMPLE_DECL1: {
			/* this may need to be synthesized */
			enum type t = get_type(get_token(n, 0)->category);

			char *k = NULL;
			struct typeinfo *v = NULL;

			struct tree *init_decl = tree_index(n, 1);
			switch (tree_size(init_decl)) {
			case 2: { /* simple variable */
				k = get_token(init_decl, 0)->text;
				v = typeinfo_new(t, false, 0);
				break;
			}
			case 3: { /* simple variable with initalizer in child 1 */
				k = get_token(init_decl, 0)->text;
				v = typeinfo_new(t, false, 0);
				break;
			}
			case 4: { /* simple pointer variable */
				k = get_token(tree_index(init_decl, 0), 1)->text;
				v = typeinfo_new(t, true, 0);
				break;
			}
			case 6: { /* simple array with size */
				k = get_token(tree_index(init_decl, 0), 0)->text;
				size_t size = get_token(tree_index(init_decl, 0), 2)->ival;
				v = typeinfo_new(ARRAY_T, 2, size, t);
				break;
			}
			}

			/* case FUNCTION_T: { */
			/* 	v = typeinfo_new(t, 3 /\*, type, typeinfo list, symbols *\/ ); */
			/* } */
			/* case CLASS_T: { */
			/* 	v = typeinfo_new(t, 2 /\*, type from typenames, symbols *\/ ); */
			/* } */
				
			if (check_declared(yyscopes, k))
				semantic_error("identifier already declared", n);
			fprintf(stderr, "inserting ident %s into table\n", k);
			insert_symbol(k, v);
		}
		default: {
			/* enqueue children */
			struct list_node *iter = list_head(n->children);
			while (!list_end(iter)) {
				list_push_back(queue, iter->data);
				iter = iter->next;
			}
		}
		}
	}
	list_free(queue);
	list_free(yyscopes);

	return global;
}	

void free_symbols(struct hash_node *n)
{
	free(n->key);
	typeinfo_delete(n->value);
}
