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

/* from lexer */
extern struct hasht *typenames;
extern bool usingstd;
extern bool fstream;
extern bool iostream;
extern bool string;

/* stack of scopes */
struct list *yyscopes;

#define scope_current() (struct hasht *)list_back(yyscopes)
#define scope_push(s) list_push_back(yyscopes, s)
#define scope_pop() list_pop_back(yyscopes)
#define scope_search(k) list_push_back(yyscopes, get_scope(scope_current(), k))

/* syntax tree helpers */
#define get_rule(n) *(enum rule *)n->data
#define get_token(n, i) ((struct token *)tree_index(n, i)->data)

/* local functions */
struct hasht *get_scope(struct hasht *s, char *k, bool private);

enum type map_type(enum yytokentype t);
char *print_basetype(struct typeinfo *t);
void print_typeinfo(FILE *stream, char *k, struct typeinfo *v);

struct hasht *symbol_populate(struct tree *syntax);
void *symbol_search(char *k);
void symbol_insert(char *k, struct typeinfo *v, struct tree *n, struct hasht *l);
void symbol_free(struct hash_node *n);

struct token *get_category(struct tree *n, int target, int before);
struct token *get_category_(struct tree *n, int target, int before);
char *get_identifier(struct tree *n);
bool get_pointer(struct tree *n);
int get_array(struct tree *n);
char *get_class(struct tree *n);
bool get_public(struct tree *n);
bool get_private(struct tree *n);

struct typeinfo *typeinfo_new(struct tree *n);
struct typeinfo *typeinfo_new_array(struct tree *n, struct typeinfo *t);
struct typeinfo *typeinfo_new_function(struct tree *n, struct typeinfo *t, bool define);
void typeinfo_delete(struct typeinfo *t);
bool typeinfo_compare(struct typeinfo *a, struct typeinfo *b);
bool typeinfo_list_compare(struct list *a, struct list *b);

bool handle_node(struct tree *n, int d);
void handle_init(struct typeinfo *v, struct tree *n);
void handle_init_list(struct typeinfo *v, struct tree *n);
void handle_function(struct typeinfo *t, struct tree *n);
void handle_param(struct typeinfo *v, struct tree *n, struct hasht *s, struct list *l);
void handle_param_list(struct tree *n, struct hasht *s, struct list *l);
void handle_class(struct typeinfo *t, struct tree *n);

void semantic_error(char *s, struct tree *n);

/*
 * Given a scope and key, get the nested scope for the key.
 */
struct hasht *get_scope(struct hasht *s, char *k, bool private)
{
	struct typeinfo *t = hasht_search(s, k);
	if (t == NULL)
		return NULL;

	switch (t->base) {
	case FUNCTION_T:
		return t->function.symbols;
	case CLASS_T:
		return (private) ? t->class.private : t->class.public;
	default:
		return NULL; /* error */
	}
}

/*
 * Maps a Bison type to a 120++ type.
 */
enum type map_type(enum yytokentype t)
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
	case CLASS_NAME:
		return CLASS_T;
	case VOID:
		return VOID_T;
	default:
		return UNKNOWN_T;
	}
}
/*
 * Given a type enum, returns its name as a static string.
 */
#define R(rule) case rule: return #rule
char *print_basetype(struct typeinfo *t)
{
	switch (t->base) {
		R(INT_T);
		R(DOUBLE_T);
		R(CHAR_T);
		R(BOOL_T);
		R(ARRAY_T);
		R(VOID_T);
		R(UNKNOWN_T);
	case FUNCTION_T:
		return print_basetype(t->function.type);
	case CLASS_T:
		return (t->class.type) ? t->class.type : "CLASS_T";
	}

	return NULL; /* error */
}
#undef R

/*
 * Prints a realistic reprensentation of a symbol to the stream.
 *
 * Example: DOUBLE_T foobar(INT_T *, AClass)
 */
void print_typeinfo(FILE *stream, char *k, struct typeinfo *v)
{
	switch (v->base) {
	case INT_T:
	case DOUBLE_T:
	case CHAR_T:
	case BOOL_T:
	case VOID_T: {
		fprintf(stream, "%s", print_basetype(v));
		break;
	}
	case ARRAY_T: {
		print_typeinfo(stream, NULL, v->array.type);
		if (k)
			fprintf(stream, " %s%s",
			        (v->array.type->pointer) ? "*" : "", k);
		if (v->array.size)
			fprintf(stream, "[%zu]", v->array.size);
		else
			fprintf(stream, "[]");
		if (k)
			fprintf(stream, "\n");
		return;
	}
	case FUNCTION_T: {
		print_typeinfo(stream, NULL, v->function.type);
		fprintf(stream, " %s%s(",
		        (v->function.type->pointer) ? "*" : "", k);

		struct list_node *iter = list_head(v->function.parameters);
		while (!list_end(iter)) {
			struct typeinfo *p = iter->data;
			print_typeinfo(stream, NULL, p);
			fprintf(stream, "%s", (p->pointer) ? " *" : "");
			iter = iter->next;
			if (!list_end(iter))
				fprintf(stream, ", ");
		}
		fprintf(stream, ")\n");
		return;
	}
	case CLASS_T: {
		fprintf(stream, "%s", print_basetype(v));
		break;
	}
	case UNKNOWN_T: {
		fprintf(stream, "unknown type");
		break;
	}
	}
	if (k)
		fprintf(stream, " %s%s\n", (v->pointer) ? "*" : "", k);
}

/*
 * Populate a global symbol table given the root of a syntax tree.
 */
struct hasht *symbol_populate(struct tree *syntax)
{
	struct hasht *global = hasht_new(32, true, NULL, NULL, &symbol_free);

	/* initialize scope stack */
	yyscopes = list_new(NULL, NULL);
	scope_push(global);

	/* handle standard libraries */
	if (usingstd) {
		if (fstream) {
			struct typeinfo *ifstream = malloc(sizeof(*ifstream));
			ifstream->base = CLASS_T;
			ifstream->class.type = "std::basic_ifstream";
			symbol_insert("ifstream", ifstream, NULL, NULL);

			struct typeinfo *ofstream = malloc(sizeof(*ofstream));
			ofstream->base = CLASS_T;
			ofstream->class.type = "std::basic_ofstream";
			symbol_insert("ofstream", ofstream, NULL, NULL);
		}
		if (iostream) {
			struct typeinfo *cin = malloc(sizeof(*cin));
			cin->base = CLASS_T;
			cin->class.type = "std::basic_istream";
			symbol_insert("cin", cin, NULL, NULL);

			struct typeinfo *cout = malloc(sizeof(*cout));
			cout->base = CLASS_T;
			cout->class.type = "std::basic_ostream";
			symbol_insert("cout", cout, NULL, NULL);

			struct typeinfo *endl = malloc(sizeof(*endl));
			endl->base = CLASS_T;
			endl->class.type = "std::basic_ostream";
			symbol_insert("endl", endl, NULL, NULL);
		}
		if (string) {
			struct typeinfo *string = malloc(sizeof(*string));
			string->base = CLASS_T;
			string->class.type = "std::string";
			symbol_insert("string", string, NULL, NULL);
		}
	}

	/* do a top-down pre-order traversal to populate symbol tables */
	tree_preorder(syntax, 0, &handle_node);

	list_free(yyscopes);

	return global;
}

/*
 * Search the stack of scopes for a given identifier.
 */
void *symbol_search(char *k)
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

/*
 * Insert symbol as ident/typeinfo pair.
 *
 * Tree used to find token on error. If attempting to insert a
 * function symbol, if the function has been previously declared, it
 * will define the function with the given symbol table. Will error
 * for duplicate symbols or mismatched function declarations.
 */
void symbol_insert(char *k, struct typeinfo *v, struct tree *n, struct hasht *l)
{
	struct typeinfo *e = symbol_search(k);
	if (e == NULL) {
		fprintf(stderr, "inserting ");
		print_typeinfo(stderr, k, v);
		hasht_insert(scope_current(), k, v);
	} else if (e->base == FUNCTION_T && v->base == FUNCTION_T) {
		if (!typeinfo_list_compare(e->function.parameters, v->function.parameters)) {
			semantic_error("function prototypes mismatched", n);
		} else if (l) {
			if (e->function.symbols == NULL)
				e->function.symbols = l;
			else
				semantic_error("function already defined", n);
		}
	} else if (e->base == CLASS_T && v->base == CLASS_T) {
		fprintf(stderr, "classes not implemented yet\n");
	} else {
		semantic_error("identifier already declared", n);
	}
}

/*
 * Frees key and deletes value.
 */
void symbol_free(struct hash_node *n)
{
	free(n->key);
	typeinfo_delete(n->value);
}

/*
 * Wrapper to return null if token not found, else return token.
 */
struct token *get_category(struct tree *n, int target, int before)
{
	struct token *t = get_category_(n, target, before);
	if (t && t->category == target)
		return t;
	else
		return NULL;
}

/*
 * Walks tree returning first token matching category, else null.
 */
struct token *get_category_(struct tree *n, int target, int before)
{
	if (tree_size(n) == 1) {
		struct token *t = n->data;
		if (t->category == target || t->category == before)
			return t;
	}

	struct list_node *iter = list_head(n->children);
	while (!list_end(iter)) {
		struct token *t = get_category_(iter->data, target, before);
		if (t && (t->category == target || t->category == before))
			return t;
		iter = iter->next;
	}

	return NULL;
}

/*
 * Returns true identifier if found, else null.
 */
char *get_identifier(struct tree *n)
{
	struct token *t = get_category(n, IDENTIFIER, -1);
	if (t)
		return t->text;
	else
		return NULL;
}

/*
 * Returns if pointer is found in tree.
 */
bool get_pointer(struct tree *n)
{
	return get_category(n, '*', IDENTIFIER);
}

/*
 * Returns size of array if found, 0 if not given, -1 if not array.
 */
int get_array(struct tree *n)
{
	if (get_category(n, '[', INTEGER)) {
		struct token *t = get_category(n, INTEGER, -1);
		return t ? t->ival : 0;
	}
	return -1;
}

/*
 * Returns class name if found, else null.
 */
char *get_class(struct tree *n)
{
	struct token *t = get_category(n, CLASS_NAME, IDENTIFIER);
	if (t)
		return t->text;
	else
		return NULL;
}

bool get_public(struct tree *n)
{
	return get_category(n, PUBLIC, PRIVATE);
}

bool get_private(struct tree *n)
{
	return get_category(n, PRIVATE, PUBLIC);
}

/*
 * Constructs new empty typeinfo.
 */
struct typeinfo *typeinfo_new(struct tree *n)
{
	struct typeinfo *t = calloc(1, sizeof(*t));
	t->base = map_type(get_token(n, 0)->category);
	t->pointer = get_pointer(n);
	if (t->base == CLASS_T)
		t->class.type = get_class(n);

	return t;
}

/*
 * Constructs a typeinfo for an array.
 */
struct typeinfo *typeinfo_new_array(struct tree *n, struct typeinfo *t)
{
	struct typeinfo *array = typeinfo_new(n);
	array->base = ARRAY_T;
	array->array.type = t;
	array->array.size = get_array(n);
	return array;
}

/*
 * Constructs a typeinfo for a function.
 */
struct typeinfo *typeinfo_new_function(struct tree *n, struct typeinfo *t, bool define)
{
	/* make new symbol table if defining */
	struct hasht *local = (define)
		? hasht_new(8, true, NULL, NULL, &symbol_free)
		: NULL;

	struct list *params = list_new(NULL, NULL);
	struct tree *m = tree_index(n, 1);

	/* add parameters (if they exist) to local symbol table */
	if (define && tree_size(m) > 2) /* definition */
		handle_param_list(tree_index(m, 1), local, params);
	if (!define && tree_size(n) > 2) /* declaration */
		handle_param_list(tree_index(n, 1), local, params);

	struct typeinfo *function = typeinfo_new(n);
	function->base = FUNCTION_T;
	function->function.type = t;
	function->function.parameters = params;
	function->function.symbols = local;

	return function;
}

/*
 * Frees types for arrays and functions, and parameter lists.
 */
void typeinfo_delete(struct typeinfo *t)
{
	switch (t->base) {
	case ARRAY_T: {
		typeinfo_delete(t->array.type);
		return;
	}
	case FUNCTION_T: {
		list_free(t->function.parameters);
		typeinfo_delete(t->function.type);
		return;
	}
	default:
		return;
	}
}

/*
 * Recursively compares two typeinfos.
 */
bool typeinfo_compare(struct typeinfo *a, struct typeinfo *b)
{
	/* Two null types are the same */
	if (a == NULL && b == NULL)
		return true;

	/* Null is unlike not null */
	if (a == NULL || b == NULL)
		return false;

	/* Same base type */
	if (a->base != b->base)
		return false;

	/* Both pointers (or not) */
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

		if (!typeinfo_list_compare(a->function.parameters, b->function.parameters))
			return false;

		return true;
	}
	case CLASS_T: {
		return (strcmp(a->class.type, b->class.type) == 0);
	}
	default:
		return true;
	}
}

/*
 * Compares each type of two lists of types (for functions).
 */
bool typeinfo_list_compare(struct list *a, struct list *b)
{
	struct list_node *a_iter = list_head(a);
	struct list_node *b_iter = list_head(b);

	while (!list_end(a_iter) && !list_end(b_iter)) {
		if (!typeinfo_compare(a_iter->data, b_iter->data))
			return false;

		a_iter = a_iter->next;
		b_iter = b_iter->next;
	}
	return list_end(a_iter) && list_end(b_iter);
}

/*
 * Recursively handles nodes, processing SIMPLE_DECL1 and
 * FUNCTION_DEF2 for symbols.
 */
bool handle_node(struct tree *n, int d)
{
	switch (get_rule(n)) {
	case SIMPLE_DECL1: {
		/* this may need to be synthesized */
		handle_init_list(typeinfo_new(n), tree_index(n, 1));
		return false;
	}
	case FUNCTION_DEF2: {
		handle_function(typeinfo_new(n), n);
		return false;
	}
	case CLASS_SPEC: {
		handle_class(typeinfo_new(n), n);
		return false;
	}
	default: /* rule did not provide a symbol, so recurse on children */
		return true;
	}
}

/*
 * Handles an init declarator recursively.
 *
 * Works for basic types, pointers, arrays, and function declarations.
 */
void handle_init(struct typeinfo *v, struct tree *n)
{
	char *k = get_identifier(n);

	switch (get_rule(n)) {
	case INIT_DECL:
	case UNARY_EXPR4:
	case DECL2:
	case MEMBER_DECL1:
	case MEMBER_DECLARATOR1: {
		/* recurse if necessary (for pointers) */
		struct list_node *iter = list_head(n->children);
		while (!list_end(iter)) {
			if (tree_size(iter->data) != 1) {
				handle_init(v, iter->data);
				return;
			}
			iter = iter->next;
		}
		/* might not recurse */
		break;
	}
	case DIRECT_DECL6: { /* array with size */
		v = typeinfo_new_array(n, v);
		break;
	}
	case DIRECT_DECL2: { /* function declaration */
		v = typeinfo_new_function(n, v, false);
		break;
	}
	case DIRECT_DECL3: { /* class constructor */
		asprintf(&k, "%s_ctor", get_class(n));
		v->base = CLASS_T;
		v->class.type = get_class(n);
		v = typeinfo_new_function(n, v, false);
		break;
	}
	default:
		semantic_error("unsupported init declaration", n);
	}
	if (k && v)
		symbol_insert(k, v, n, NULL);
	else
		semantic_error("failed to get init declarator symbol", n);
}

/*
 * Handles lists of init declarators recursively.
 */
void handle_init_list(struct typeinfo *v, struct tree *n)
{
	enum rule r = get_rule(n);
	if (r == INIT_DECL_LIST2 || r == MEMBER_SPEC1 || r == MEMBER_DECL_LIST2) {
		/* recurse through lists of declarators */
		struct list_node *iter = list_head(n->children);
		while (!list_end(iter)) {
			handle_init_list(v, iter->data);
			iter = iter->next;
		}
	} else if (r == MEMBER_SPEC2) {
		/* begining of class access specifier tree */
		if (get_public(n)) {
			v->class.public = hasht_new(8, true, NULL, NULL, &symbol_free);
			scope_push(v->class.public);
		} else if (get_private(n)) {
			v->class.private = hasht_new(8, true, NULL, NULL, &symbol_free);
			scope_push(v->class.private);
		} else {
			semantic_error("unrecognized class access specifier", n);
		}
		handle_init_list(typeinfo_new(n), tree_index(n, 1));
		scope_pop();
	} else if (r == MEMBER_DECL1) {
		/* recurse to end of class declaration */
		handle_init_list(typeinfo_new(n), tree_index(n, 1));
	} else {
		/* process a single declaration */
		v->pointer = get_pointer(n); /* for pointers in lists */
		handle_init(v, n);
	}
}

/*
 * Handles function definitions, recursing with handle_node().
 */
void handle_function(struct typeinfo *t, struct tree *n)
{
	char *k = get_identifier(n);
	struct typeinfo *v = typeinfo_new_function(n, t, true);
	symbol_insert(k, v, n, v->function.symbols);

	/* recurse on children while in subscope */
	scope_push(v->function.symbols);
	tree_preorder(tree_index(n, 2), 0, &handle_node);
	scope_pop();
}

/*
 * Handles a PARAM_DECL1 or a PARAM_DECL3 rules.
 *
 * Works for basic types, pointers, and arrays. If given a scope hash
 * table and able to find an indentifier, inserts into the scope. If
 * given a parameters list, will insert into the list.
 */
void handle_param(struct typeinfo *v, struct tree *n, struct hasht *s, struct list *l)
{
	char *k = get_identifier(n);

	if (tree_size(n) > 3) { /* not a simple type */
		struct tree *m = tree_index(n, 1);
		enum rule r = get_rule(m);

		/* array (with possible size) */
		if (r == DIRECT_ABSTRACT_DECL4 || r == DIRECT_DECL6)
			v = typeinfo_new_array(m, v);
	}

	if (l && v)
		list_push_back(l, v);
	else if (s && k && v)
		hasht_insert(s, k, v);
	else
		semantic_error("unsupported parameter declaration", n);
}

/*
 * Handles an arbitrarily nested list of parameters recursively.
 */
void handle_param_list(struct tree *n, struct hasht *s, struct list *l)
{
	enum rule r = get_rule(n);
	if (r == PARAM_DECL1 || r == PARAM_DECL3) {
		struct typeinfo *v = typeinfo_new(n);
		v->pointer = get_pointer(n); /* for pointers in list */
		handle_param(v, n, s, l);
	} else {
		struct list_node *iter = list_head(n->children);
		while (!list_end(iter)) {
			handle_param_list(iter->data, s, l);
			iter = iter->next;
		}
	}
}

/*
 * Handles a class declaration with public and/or private scopes.
 */
void handle_class(struct typeinfo *t, struct tree *n)
{
	char *k = get_identifier(n);
	t->base = CLASS_T; /* class definition is still a class */
	symbol_insert(k, t, n, NULL);
	handle_init_list(t, tree_index(n, 1));
}

/*
 * Follow a node to a token, emit error, exit with 3.
 */
void semantic_error(char *s, struct tree *n)
{
	while (tree_size(n) > 1)
		n = list_front(n->children);
	struct token *t = n->data;
	fprintf(stderr, "Semantic error: file %s, line %d, token %s: %s\n",
	        t->filename, t->lineno, t->text, s);
	exit(3);
}
