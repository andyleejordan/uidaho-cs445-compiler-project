/*
 * hasht.h - Interface for dynamically expanding hash table.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

#ifndef HASHT_H
#define HASHT_H

#include <stddef.h>
#include <stdbool.h>

bool HASHT_DEBUG;

struct hasht_node {
	void *key;
	void *value;
};

struct hasht {
	struct hasht_node **table;
	size_t size;
	size_t used;
	bool grow;
	size_t (*hash)(void *key, int perm);
	bool (*compare)(void *a, void *b);
	void (*delete)(struct hasht_node *n);
};

struct hasht *hasht_new(size_t size,
                        bool grow,
                        size_t (*hash)(void *key, int perm),
                        bool (*compare)(void *a, void *b),
                        void (*delete)(struct hasht_node *n));

void *hasht_insert(struct hasht *self, void *key, void *value);
void *hasht_search(struct hasht *self, void *key);
void *hasht_delete(struct hasht *self, void *key);

size_t hasht_size(struct hasht *self);
size_t hasht_used(struct hasht *self);

void hasht_resize(struct hasht *self, size_t size);
void hasht_free(struct hasht *self);

struct hasht_node *hasht_node_new(struct hasht_node *n, void *key, void *value);
bool hasht_node_deleted(struct hasht_node *n);

#endif /* HASHT_H */
