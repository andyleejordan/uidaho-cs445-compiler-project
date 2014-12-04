/*
 * hasht.c - Unit test code for hash table.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "test.h"
#include "list.h"
#include "hasht.h"

struct hasht *test_hasht_new(size_t size);
void test_hasht_insert(struct hasht *t, char *k, char *v);
void test_hasht_search(struct hasht *t, char *k, char *v);
void test_hasht_insert_duplicate(struct hasht *t, char *k, char *v);
void test_hasht_used(struct hasht *t, size_t s);
void test_hasht_resize(struct hasht *t, size_t size);
void test_hasht_delete(struct hasht *t, char *k, char *v);

int main()
{
	running("hash table");

	testing("new");
	size_t size = 4;
	struct hasht *t = test_hasht_new(size);

	char *k = strdup("foo");
	char *v = strdup("bar");

	testing("insert");
	test_hasht_insert(t, k, v);

	testing("search");
	test_hasht_search(t, k, v);

	testing("insert duplicate");
	test_hasht_insert_duplicate(t, k, v);

	testing("size");
	char *k2 = strdup("the answer?");
	char *v2 = strdup("42");
	test_hasht_insert(t, k2, v2);
	test_hasht_used(t, 2);

	testing("resize");
	test_hasht_resize(t, size);

	testing("remove");
	test_hasht_delete(t, k, v);

	hasht_free(t);
}

struct hasht *test_hasht_new(size_t size) {
	struct hasht *t = hasht_new(size, true, NULL, NULL, NULL);
	assert(hasht_size(t) == size);
	return t;
}

void test_hasht_insert(struct hasht *t, char *k, char *v)
{
	struct hasht_node *slot = hasht_insert(t, k, v);
	assert(slot->value == v);
}

void test_hasht_search(struct hasht *t, char *k, char *v)
{
	void *test_v = hasht_search(t, k);
	assert(test_v == v);
}

void test_hasht_insert_duplicate(struct hasht *t, char *k, char *v)
{
	hasht_insert(t, k, v);
	void *test_v = hasht_insert(t, k, v);
	assert(test_v == NULL);
}

void test_hasht_used(struct hasht *t, size_t s)
{
	assert(hasht_used(t) == s);
}

void test_hasht_resize(struct hasht *t, size_t size)
{
	hasht_insert(t, strdup("one"), strdup("1"));
	hasht_insert(t, strdup("two"), strdup("2"));
	/* table should have doubled in size once */
	assert(hasht_size(t) == size * 2);
	test_hasht_used(t, 4);
}

void test_hasht_delete(struct hasht *t, char *k, char *v)
{
	char *test_v = hasht_delete(t, k);
	assert(v == test_v);
	free(v);
}
