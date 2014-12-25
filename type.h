/*
 * type.h - Definition of an abstract 120++ type.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "token.h"
#include "parser.tab.h"

struct tree;
struct list;
struct hasht;

/* the 120++ base types */
enum type {
	INT_T,
	FLOAT_T,
	CHAR_T,
	BOOL_T,
	ARRAY_T,
	FUNCTION_T,
	CLASS_T,
	VOID_T,
	UNKNOWN_T
};

void set_type_comparators();
enum type map_type(enum yytokentype t);
char *print_type(enum type t);

/* memory regions and address type */
enum region {
	GLOBE_R,
	LOCAL_R,
	PARAM_R,
	CLASS_R,
	LABEL_R,
	CONST_R,
	UNKNOWN_R
};

char *print_region(enum region r);

/* global region and offset in main */
extern enum region region;
extern size_t offset;

struct typeinfo;

struct address {
	enum region region;
	int offset;
	struct typeinfo *type;
};

void print_address(FILE *stream, struct address a);

/* abstract representation of a 120++ type */
struct typeinfo {
	enum type base;
	bool pointer;
	struct address place;
	struct token *token;

	union {
		struct arrayinfo {
			struct typeinfo *type;
			size_t size;
		} array;
		struct functioninfo {
			struct typeinfo *type; /* return */
			struct list *parameters; /* typeinfo */
			size_t param_size; /* total bytes of parameters */
			struct hasht *symbols; /* NULL until defined */
		} function;
		struct classinfo {
			char *type; /* from yytypes table */
			struct hasht *public;
			struct hasht *private;
		} class;
	};
};

struct typeinfo *typeinfo_new(struct tree *n);
struct typeinfo *typeinfo_new_array(struct tree *n, struct typeinfo *t);
struct typeinfo *typeinfo_new_function(struct tree *n, struct typeinfo *t, bool define);
struct typeinfo *typeinfo_copy(struct typeinfo *t);
void typeinfo_delete(struct typeinfo *t);

struct typeinfo *typeinfo_return(struct typeinfo *t);
size_t typeinfo_size(struct typeinfo *t);

bool typeinfo_compare(struct typeinfo *a, struct typeinfo *b);
bool typeinfo_list_compare(struct list *a, struct list *b);

char *print_basetype(struct typeinfo *t);
void print_typeinfo(FILE *stream, const char *k, struct typeinfo *v);

#endif /* TYPE_H */
