/*
 * symbol.c - Implementation of symbol data.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "symbol.h"
#include "list.h"
#include "hasht.h"

/*
 * Represents a scope as a pairing of name and hashe table of symbols.
 *
 * Name of the scope is "global" or "some_function" or "SomeClass",
 * etc. A symbol's key is its name, and its value is a typeinfo
 * pointer.
 *
 * hasht_insert(scope->symbols, symbol_name, symbol_type);
 * tree_push(parent_scope, scope_new(child_scope));
 */
struct scope *scope_new(char *name)
{
	struct scope *s = malloc(sizeof(*s));
	s->name = name; /* use flyweight table later */
	s->symbols = hasht_new(2, true, NULL, NULL, NULL);
	return s;
}	

/*
 * Constructs new typeinfo.
 *
 * array: count 2, size, typeinfo
 * function: count 3, typeinfo, parameters list, scope
 * class: count 2, name, scope
 */
struct typeinfo *typeinfo_new(enum type base, int count, ...)
{
	va_list ap;
	va_start(ap, count);

	struct typeinfo *t = malloc(sizeof(*t));
	t->base = base;

	switch (t->base) {
	case ARRAY: {
		t->array.size = va_arg(ap, size_t);
		t->array.type = va_arg(ap, struct typeinfo *);
	}
	case FUNCTION: {
		t->function.type = va_arg(ap, struct typeinfo *);
		t->function.parameters = va_arg(ap, struct list *);
		t->function.symbols = va_arg(ap, struct scope *);
	}
	case CLASS: {
		t->class.name = va_arg(ap, char *);
		t->class.symbols = va_arg(ap, struct scope *);
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
	case ARRAY:
		typeinfo_delete(t->array.type);
	case FUNCTION:
		list_free(t->function.parameters);
	case CLASS:
		free(t->class.name);
	default:
		break;
	}
}	
