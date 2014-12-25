/*
 * scope.c -
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

#include <stddef.h>

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

/*
 * Returns sum of sizes of symbols in scope.
 *
 * This is sadly highly coupled with my hash table implementation
 * since it does not (yet) have an iterable interface.
 */
size_t scope_size(struct hasht *t)
{
	if (t == NULL)
		return 0;

	size_t total = 0;
	for (size_t i = 0; i < t->size; ++i) {
		struct hasht_node *slot = t->table[i];
		if (slot && !hasht_node_deleted(slot)) {
			total += typeinfo_size(slot->value);
		}
	}
	return total;
}
