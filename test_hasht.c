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

struct hasht *test_hasht_new();
void test_hasht_insert_retrieve(struct hasht *table, char *key, char *value);
void test_hasht_insert_again(struct hasht *table, char *key, char *value);
void test_hasht_delete(struct hasht *table, char *key, char *value);

int main()
{
	running("hash table");

	testing("new");
	struct hasht *table = test_hasht_new();

	char *key = strdup("foo");
	char *value = strdup("bar");
	
	testing("insert/retrieve");
	test_hasht_insert_retrieve(table, key, value);

	testing("insert duplicate");
	test_hasht_insert_again(table, key, value);

	testing("remove");
	test_hasht_delete(table, key, value);
	free(key);
}

struct hasht *test_hasht_new() {
	size_t size = 64;

	struct hasht *table = hasht_new(size);

	assert(table_size(table) == size);
	assert(sizeof(*table)/sizeof(struct list_node*) == size);

	return table;
}
void test_hasht_insert_retrieve(struct hasht *table, char *key, char *value)
{
	struct hash_node *hash = hasht_insert(table, key, value);
	assert(hash->key == key);
	assert(hash->value == value);
}

void test_hasht_insert_again(struct hasht *table, char *key, char *value)
{
	hasht_insert(table, key, value);
	struct hash_node *hash = hasht_insert(table, key, value);
	assert(hash == NULL);

}

void test_hasht_delete(struct hasht *table, char *key, char *value)
{
	char *test_value = hasht_delete(table, key);
	assert(value == test_value);
	free(value);
}
