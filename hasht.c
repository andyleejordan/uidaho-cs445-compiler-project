/*
 * hasht.c - Source code for dynamically expanding hash table.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hasht.h"

size_t hasht_hash(struct hasht *self, void *key, int perm);

bool hasht_default_compare(void *a, void *b);
size_t hasht_default_hash(char *key, int perm);
void hasht_default_delete(struct hash_node *n);

/*
 * Dynamically allocate n array of null hash_node pointers.
 *
 * If given null for hash, compare, or delete functions, uses default.
 */
struct hasht *hasht_new(size_t size,
                        size_t (*hash)(void *key, int perm),
                        bool (*compare)(void *a, void *b),
                        void (*delete)(struct hash_node *n))
{
	struct hasht *t = malloc(sizeof(*t));
	if (t == NULL) {
		perror("hasht_new()");
		return NULL;
	}

	t->table = calloc(size, sizeof(t->table));
	if (t->table == NULL) {
		perror("hasht_new()");
		return NULL;
	}

	t->size = size == 0
		? 256
		: size;

	t->used = 0;

	t->hash = (hash == NULL)
		? (size_t (*)(void *, int perm))&hasht_default_hash
		: hash;

	t->compare = (compare == NULL)
		? &hasht_default_compare
		: compare;

	t->delete = (delete == NULL)
		? &hasht_default_delete
		: delete;

	return t;
}

/*
 * Inserts value into slot corresponding to key and availability.
 *
 * Collisions are handled with open-addressing. Nodes marked deleted
 * are reused by hash_node_new(). Returns null if key already present
 * or table is full.
 */
void *hasht_insert(struct hasht *self, void *key, void *value)
{
	if (self == NULL) {
		fprintf(stderr, "hasht_insert(): self was null\n");
		return NULL;
	}

	for(size_t i = 0; i < hasht_size(self); ++i) {
		size_t index = hasht_hash(self, key, i);
		struct hash_node **slot = &self->table[index];
		if (*slot == NULL || hash_node_deleted(*slot)) {
			++self->used;
			*slot = hash_node_new(*slot, key, value);
			return value;
		} else if (self->compare(key, (*slot)->key)) {
			return NULL;
		}
	}

	fprintf(stderr, "hasht_insert(): failed (table full?)\n");
	return NULL;
}

/*
 * Returns value corresponding to key if found, else null.
 *
 * Searches through permutations until null is reached. Deleted items
 * are not null, but have null keys, which must not be used otherwise.
 */
void *hasht_search(struct hasht *self, void *key)
{
	for(size_t i = 0; i < hasht_size(self); ++i) {
		size_t index = hasht_hash(self, key, i);
		struct hash_node *slot = self->table[index];
		if (slot == NULL)
			return NULL;
		else if (self->compare(key, slot->key))
			return slot->value;
	}
	return NULL;
}

/*
 * Marks node corresponding to key as deleted by setting key to null.
 *
 * Do *not* try to use null as a valid key.
 */
void *hasht_delete(struct hasht *self, void *key)
{
	for(size_t i = 0; i < hasht_size(self); ++i) {
		size_t index = hasht_hash(self, key, i);
		struct hash_node *slot = self->table[index];
		if (slot == NULL) { 
			return NULL; /* key not in table */
		} else if (self->compare(key, slot->key)) {
			slot->key = NULL; /* mark deleted */
			return slot->value;
		}
	}
	return NULL; /* table full and key not in table */
}

/*
 * Helper function to return max table size.
 */
size_t hasht_size(struct hasht *self)
{
	if (self == NULL) {
		fprintf(stderr, "hasht_size(): self was null\n");
		return 0;
	}
	return self->size;
}

/*
 * Helper function to return actual table size.
 */
size_t hasht_used(struct hasht *self)
{
	if (self == NULL) {
		fprintf(stderr, "hasht_used(): self was null\n");
		return 0;
	}
	return self->used;
}

/*
 * Frees all allocated nodes, then frees node array, then frees self.
 */
void hasht_free(struct hasht *self)
{
	for (size_t i = 0; i < hasht_size(self); ++i) {
		struct hash_node *slot = self->table[i];
		if (slot != NULL)
			self->delete(slot);
	}
	free(self->table);
	free(self);
}

/*
 * Maps key to table index.
 */
size_t hasht_hash(struct hasht *self, void *key, int perm)
{
	if (self == NULL) {
		fprintf(stderr, "hasht_hash(): self was null\n");
		return perm;
	}
	return self->hash(key, perm) % hasht_size(self);
}

/*
 * Default hash. Work in progress.
 */
size_t hasht_default_hash(char *key, int perm)
{
	return strlen(key) + perm;
}

/*
 * Default comparison for string keys.
 */
bool hasht_default_compare(void *a, void *b)
{
	return (strcmp((char *)a, (char *)b) == 0);
}

/*
 * Default function for freeing a node.
 */
void hasht_default_delete(struct hash_node *n)
{
	if (n == NULL) {
		fprintf(stderr, "hasht_default_delete(): node was null\n");
		return;
	}
	if (n->key) {
		free(n->key);
		free(n->value);
	}
	free(n);
}

/*
 * Allocates hash_node for key / value pair.
 */
struct hash_node *hash_node_new(struct hash_node *n, void *key, void *value)
{
	if (n == NULL)
		n = malloc(sizeof(*n));

	n->key = key;
	n->value = value;

	return n;
}

/*
 * Returns true if key is null, thus marked as deleted.
 */
bool hash_node_deleted(struct hash_node *n)
{
	if (n == NULL) {
		fprintf(stderr, "hasht_node_deleted(): node was null\n");
		return true;
	}
	return (n->key == NULL);
}
