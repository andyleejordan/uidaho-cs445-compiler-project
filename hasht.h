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
	struct hash_node **table;
	size_t size;
	size_t used;
	size_t (*hash)(void *key, int perm);
	bool (*compare)(void *a, void *b);
	void (*delete)(struct hash_node *n);
};

struct hasht *hasht_new(size_t size,
                        size_t (*hash)(void *key, int perm),
                        bool (*compare)(void *a, void *b),
                        void (*free)(struct hash_node *n));

void *hasht_insert(struct hasht *self, void *key, void *value);
void *hasht_search(struct hasht *self, void *key);
void *hasht_delete(struct hasht *self, void *key);

size_t hasht_size(struct hasht *self);
size_t hasht_used(struct hasht *self);

void hasht_free(struct hasht *self);

struct hash_node *hash_node_new(struct hash_node *n, void *key, void *value);
bool hash_node_deleted(struct hash_node *n);

#endif /* HASHT_H */
