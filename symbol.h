/*
 * symbol.h - Semantic data for symbols including type and scope.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>
#include <stddef.h>

struct hash_node;

enum type {
	INT,
	FLOAT,
	CHAR,
	BOOL,
	ARRAY,
	FUNCTION,
	CLASS,
	VOID
};

struct scope {
	char *name;
	struct hasht *symbols;
};

struct scope *scope_new(char *name);

struct typeinfo {
	enum type base;
	union {
		struct arrayinfo {
			size_t size;
			struct typeinfo *type;
		} array;
		struct functioninfo {
			struct typeinfo *type;
			struct list *parameters;
			struct scope *symbols;
		} function;
		struct classinfo {
			char *name;
			struct scope *symbols;
		} class;
	};
};

struct typeinfo *typeinfo_new(enum type base, int count, ...);
void typeinfo_delete(struct typeinfo *n);

#endif /* SYMBOL_H */
