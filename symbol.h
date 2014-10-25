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

struct tree;

struct hasht *symbol_populate(struct tree *syntax);

enum type {
	INT_T,
	DOUBLE_T,
	CHAR_T,
	BOOL_T,
	ARRAY_T,
	FUNCTION_T,
	CLASS_T,
	VOID_T,
	UNKNOWN_T
};

struct typeinfo {
	enum type base;
	bool pointer;
	union {
		struct arrayinfo {
			struct typeinfo *type;
			size_t size;
		} array;
		struct functioninfo {
			struct typeinfo *type; /* return */
			struct list *parameters; /* typeinfo */
			struct hasht *symbols; /* NULL until defined */
		} function;
		struct classinfo {
			char *type; /* from typenames table */
			struct hasht *public;
			struct hasht *private;
		} class;
	};
};

#endif /* SYMBOL_H */
