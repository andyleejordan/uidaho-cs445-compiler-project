/*
 * type.c - Implementation of abstract type helpers.
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

#include "type.h"
#include "symbol.h"
#include "token.h"
#include "scope.h"

#include "logger.h"
#include "list.h"
#include "tree.h"
#include "hasht.h"

/* basic type comparators */
struct typeinfo int_type;
struct typeinfo float_type;
struct typeinfo char_type;
struct typeinfo string_type;
struct typeinfo bool_type;
struct typeinfo void_type;
struct typeinfo class_type;
struct typeinfo unknown_type;
struct typeinfo ptr_type;

/*
 * Initialize comparator types
 */
void set_type_comparators()
{
	int_type.base = INT_T;
	int_type.pointer = false;

	float_type.base = FLOAT_T;
	float_type.pointer = false;

	char_type.base = CHAR_T;
	char_type.pointer = false;

	string_type.base = CHAR_T;
	string_type.pointer = true; /* a C string is a char* */

	bool_type.base = BOOL_T;
	bool_type.pointer = false;

	void_type.base = VOID_T;
	void_type.pointer = false;

	class_type.base = CLASS_T;
	class_type.pointer = false;

	unknown_type.base = UNKNOWN_T;
	unknown_type.pointer = false;

	ptr_type.base = VOID_T;
	ptr_type.pointer = true;
}

/*
 * Maps a Bison type to a 120++ type.
 */
enum type map_type(enum yytokentype t)
{
	switch (t) {
	case INTEGER:
	case INT:
	case SHORT:
	case LONG:
		return INT_T;
	case FLOATING:
	case FLOAT:
	case DOUBLE:
		return FLOAT_T;
	case CHARACTER:
	case CHAR:
		return CHAR_T;
	case STRING:
		return ARRAY_T;
	case TRUE:
	case FALSE:
	case BOOL:
		return BOOL_T;
	case CLASS_NAME:
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
 * Given a region enum, returns its name as a static string.
 */
char *print_region(enum region r)
{
	switch (r) {
	case GLOBE_R:
		return "global";
	case LOCAL_R:
		return "local";
	case PARAM_R:
		return "param";
	case CLASS_R:
		return "class";
	case LABEL_R:
		return "";
	case CONST_R:
		return "const";
	case UNKNOWN_R:
		return "unknown";
	};

	return NULL; /* error */
}

/*
 * Given a stream, prints an address to it.
 */
void print_address(FILE *stream, struct address a)
{
	fprintf(stream, "(%s%s)%s:%i", print_basetype(a.type),
	        (a.type->pointer ? " *" : ""),
	        print_region(a.region), a.offset);
}

/*
 * Constructs new empty typeinfo.
 */
struct typeinfo *typeinfo_new(struct tree *n)
{
	log_assert(n);

	struct typeinfo *t = calloc(1, sizeof(*t));
	log_assert(t);

	if (get_rule(n) == CTOR_FUNCTION_DEF) {
		/* constructor return type is always class */
		t->base = CLASS_T;
	} else {
		/* otherwise traverse to first leaf node (should be type) */
		struct tree *iter = n;
		while (tree_size(iter) > 1)
			iter = list_front(iter->children);
		struct token *token = get_token(iter->data);
		t->base = map_type(token->category);

	}
	t->pointer = get_pointer(n);

	if (t->base == CLASS_T) {
		t->class.type = get_class(n);
		struct typeinfo *c = scope_search(t->class.type);
		/* if class has been defined, copy scopes */
		if (c) {
			t->class.private = c->class.private;
			t->class.public = c->class.public;
		}
	}

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
void handle_param_list(struct tree *n, struct hasht *s, struct list *l);
struct typeinfo *typeinfo_new_function(struct tree *n, struct typeinfo *t, bool define)
{
	/* make new symbol table if defining */
	struct hasht *local = (define)
		? hasht_new(8, true, NULL, NULL, &symbol_free)
		: NULL;

	struct list *params = list_new(NULL, NULL);
	log_assert(params);

	/* setup parameter region and offset */
	enum region region_ = region;
	size_t offset_ = offset;
	region = PARAM_R;
	offset = 0;

	if (class_member(n))
		offset += typeinfo_size(&ptr_type);

	handle_param_list(n, local, params);

	struct typeinfo *function = typeinfo_new(n);
	function->base = FUNCTION_T;
	function->pointer = false; /* 120++ does not have function pointers */
	function->function.type = t;
	function->function.parameters = params;
	function->function.param_size = offset;
	function->function.symbols = local;

	/* restore region and offset */
	region = region_;
	offset = offset_;

	return function;
}

/*
 * Returns a copy of a typeinfo object.
 *
 * Memory leaks galore right now.
 */
struct typeinfo *typeinfo_copy(struct typeinfo *t)
{
	log_assert(t);

	struct typeinfo *n = malloc(sizeof(*n));
	n = memcpy(n, t, sizeof(*t));
	log_assert(n);


	return n;
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
 * Returns the actual type for a typeinfo.
 *
 * For primitives and classes returns itself.
 */
struct typeinfo *typeinfo_return(struct typeinfo *t)
{
	log_assert(t);

	if (t->base == FUNCTION_T)
		return t->function.type;
	else
		return t;
}

/*
 * Returns calculated size for type. Assuming 64-bit.
 */
size_t typeinfo_size(struct typeinfo *t)
{
	if (t->pointer)
		return 8;

	switch (t->base) {
	case INT_T:
	case FLOAT_T:
		return 8;
	case CHAR_T:
	case BOOL_T:
		return 1;
	case ARRAY_T:
		return t->array.size * typeinfo_size(t->array.type);
	case FUNCTION_T:
		return scope_size(t->function.symbols);
	case CLASS_T:
		return scope_size(t->class.public) + scope_size(t->class.private);
	default:
		return 0;
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

		if (a->array.size != 0 && a->array.size != b->array.size)
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
 * Given a type, returns its name as a static string.
 */
char *print_basetype(struct typeinfo *t)
{
	log_assert(t);
	switch (t->base) {
	case INT_T:
		return "int";
	case FLOAT_T:
		return "double";
	case CHAR_T:
		return "char";
	case BOOL_T:
		return "bool";
	case ARRAY_T:
		return print_basetype(t->array.type);
	case VOID_T:
		return "void";
	case UNKNOWN_T:
		return "label";
	case FUNCTION_T:
		return print_basetype(t->function.type);
	case CLASS_T:
		return (t->class.type) ? t->class.type : "class";
	}

	return NULL; /* error */
}

/*
 * Prints a realistic reprensentation of a symbol to the stream.
 *
 * Example: double foobar(int *, AClass)
 */
void print_typeinfo(FILE *stream, const char *k, struct typeinfo *v)
{
	if (v == NULL)
		log_error("print_typeinfo(): type for %s was null", k);

	switch (v->base) {
	case INT_T:
	case FLOAT_T:
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
		fprintf(stream, ")");
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
		fprintf(stream, " %s%s", (v->pointer) ? "*" : "", k);
}
