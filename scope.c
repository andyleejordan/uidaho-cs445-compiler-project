/*
 * scope.c -
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include "scope.h"
#include "symbol.h"
#include "list.h"
#include "hasht.h"

/*
 * Search the stack of scopes for a given identifier.
 */
struct typeinfo *scope_search(char *k)
{
	if (!k)
		return NULL;

	struct list_node *iter = list_tail(yyscopes);
	while (!list_end(iter)) {
		struct typeinfo *t = hasht_search(iter->data, k);
		if (t)
			return t;
		iter = iter->prev;
	}
	return NULL;
}
