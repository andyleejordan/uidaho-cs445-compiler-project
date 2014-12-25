/*
 * hasht.c - Source code for dynamically expanding hash table.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hasht.h"

/* from lookup3.c */
void hashlittle2(const void *key, size_t length, uint32_t *pc, uint32_t *pb);

static void hasht_debug(const char *format, ...);
static size_t hasht_hash(struct hasht *self, void *key, int perm);
static uint32_t hasht_default_hash(char *key, int perm);
static bool hasht_default_compare(void *a, void *b);
static void hasht_default_delete(struct hasht_node *n);

/*
 * Dynamically allocate n array of null hasht_node pointers.
 *
 * If given null for hash, compare, or delete functions, uses default.
 */
struct hasht *hasht_new(size_t size,
                        bool grow,
                        size_t (*hash)(void *key, int perm),
                        bool (*compare)(void *a, void *b),
                        void (*delete)(struct hasht_node *n))
{
	/* ensure size is a power of 2 if using default hash */
	if (size != 0 && hash == NULL && (size & (size - 1))) {
		hasht_debug("hasht_new(): default hash requires size to be power of 2");
		return NULL;
	}

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

	t->grow = grow;

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
 * are reused by hasht_node_new(). Returns null if key already present
 * or table is full.
 */
void *hasht_insert(struct hasht *self, void *key, void *value)
{
	if (self == NULL) {
		hasht_debug("hasht_insert(): self was null");
		return NULL;
	}

	if (key == NULL) {
		hasht_debug("hasht_insert(): key was null");
		return NULL;
	}

	if (self->grow && (self->used > self->size / 2))
		hasht_resize(self, self->size * 2);

	for(size_t i = 0; i < hasht_size(self); ++i) {
		size_t index = hasht_hash(self, key, i);
		struct hasht_node **slot = &self->table[index];
		if (*slot == NULL || hasht_node_deleted(*slot)) {
			++self->used;
			*slot = hasht_node_new(*slot, key, value);
			return *slot;
		} else if (self->compare(key, (*slot)->key)) {
			return NULL;
		}
	}

	hasht_debug("hasht_insert(): failed (table full?)");
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
		struct hasht_node *slot = self->table[index];
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
		struct hasht_node *slot = self->table[index];
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
		hasht_debug("hasht_size(): self was null");
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
		hasht_debug("hasht_used(): self was null");
		return 0;
	}
	return self->used;
}

/*
 * Resize hash table by reinsertion.
 *
 * Creates a new array of given size for self. Iterates through old
 * table, deleting nodes marked "deleted", and otherwise inserting
 * existing key/value pairs into the new table. Frees old table array.
 */
void hasht_resize(struct hasht *self, size_t size)
{
	if (self->used > size) {
		hasht_debug("hasht_resize(): requested size too small");
		return;
	}

	size_t old_size = self->size;
	struct hasht_node **old_table = self->table;
	struct hasht_node **new_table = calloc(size, sizeof(*new_table));
	if (new_table == NULL) {
		perror("hasht_resize()");
		return;
	}

	self->table = new_table;
	self->size = size;
	self->used = 0; /* reset used count since insert increments it */

	for (size_t i = 0; i < old_size; ++i) {
		struct hasht_node *slot = old_table[i];
		if (slot) {
			if (hasht_node_deleted(slot))
				self->delete(slot);
			else
				hasht_insert(self, slot->key, slot->value);
		}
	}
	free(old_table);
}

/*
 * Frees all allocated nodes, then frees node array, then frees self.
 */
void hasht_free(struct hasht *self)
{
	for (size_t i = 0; i < hasht_size(self); ++i) {
		struct hasht_node *slot = self->table[i];
		if (slot)
			self->delete(slot);
	}
	free(self->table);
	free(self);
}

/*
 * Maps key to table index.
 */
static size_t hasht_hash(struct hasht *self, void *key, int perm)
{
	if (self == NULL) {
		hasht_debug("hasht_hash(): self was null");
		return perm;
	}
	return self->hash(key, perm) % hasht_size(self);
}

/*
 * Default hash. Uses double hashing for open addressing.
 *
 * Gets h1 and h2 from Jenkins' hashlittle2().
 */
static uint32_t hasht_default_hash(char *key, int perm)
{
	uint32_t h1 = 0;
	uint32_t h2 = 0;
	hashlittle2(key, strlen(key), &h1, &h2);

	/* given table size m as a power of 2, this ensures h2 will always be
	   relatively prime to m, per CLRS */
	if (h2 % 2 == 0)
		--h2;

	return (h1 + perm * h2);
}

/*
 * Default comparison for string keys.
 */
static bool hasht_default_compare(void *a, void *b)
{
	return (strcmp((char *)a, (char *)b) == 0);
}

/*
 * Default function for freeing a node.
 */
static void hasht_default_delete(struct hasht_node *n)
{
	if (n == NULL) {
		hasht_debug("hasht_default_delete(): node was null");
		return;
	}
	if (n->key) {
		free(n->key);
		free(n->value);
	}
	free(n);
}

/*
 * Allocates hasht_node for key / value pair.
 */
struct hasht_node *hasht_node_new(struct hasht_node *n, void *key, void *value)
{
	if (n == NULL) {
		n = malloc(sizeof(*n));
		if (n == NULL) {
			hasht_debug("hasht_node_new(): couldn't allocate new node");
			return NULL;
		}
	}

	n->key = key;
	n->value = value;

	return n;
}

/*
 * Returns true if key is null, thus marked as deleted.
 */
bool hasht_node_deleted(struct hasht_node *n)
{
	if (n == NULL) {
		hasht_debug("hasht_node_deleted(): node was null");
		return true;
	}
	return (n->key == NULL);
}

static void hasht_debug(const char *format, ...)
{
	if (!HASHT_DEBUG)
		return;

	va_list ap;
	va_start(ap, format);

	fprintf(stderr, "debug: ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}
