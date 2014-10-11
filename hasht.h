/*
 * hasht.h - Interface for dynamically expanding hash table.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef HASHT_H
#define HASHT_H

#include <stddef.h>
#include <stdbool.h>

struct list;

struct hash_node {
	char *key;
	void *value;
};

struct hasht {
	struct list **table;
	size_t size;
	size_t used;
};

struct hasht *hasht_new(size_t size);

struct hash_node *hasht_insert(struct hasht *self, char *key, void *value);
void *hasht_search(struct hasht *self, char *key);
void *hasht_delete(struct hasht *self, char *key);

bool hasht_contains(struct hasht *self, char *key);
size_t table_size(struct hasht *self);

#endif /* HASHT_H */
