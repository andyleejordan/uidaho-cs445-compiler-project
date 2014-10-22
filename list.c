/*
 * list.c - Source code for doubly linked circular list.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"

bool list_default_compare(void *a, void *b);

/*
 * Returns allocated list with uncounted sentinel element.
 */
struct list *list_new(bool (*compare)(void *a, void *b),
                      void (*delete)(void *data))
{
	struct list *l = malloc(sizeof(*l));
	if (l == NULL) {
		perror("list_new()");
		return NULL;
	}

	struct list_node *sentinel = list_node_new(NULL);
	if (sentinel == NULL) {
		free(l);
		return NULL;
	}

	l->sentinel = sentinel;
	l->size = 0;

	sentinel->sentinel = true;
	sentinel->next = sentinel;
	sentinel->prev = sentinel;

	l->compare = (compare == NULL)
		? &list_default_compare
		: compare;

	l->delete = delete;

	return l;
}

/*
 * Inserts data at pos in O(n/2). Returns new node.
 *
 * Position 0 inserts at the front and n inserts at the end in O(1).
 */
struct list_node *list_insert(struct list *self, int pos, void *data)
{
	struct list_node *b = list_node_new(data);
	struct list_node *c = list_index(self, pos);

	list_node_link(self, b, c);

	return b;
}

/*
 * Use compare function to return found node, else returns sentinel.
 */
struct list_node *list_search(struct list *self, void *data) {
	struct list_node *iter = list_head(self);
	while (!list_end(iter)) {
		if (self->compare(data, iter->data))
			return iter;
		iter = iter->next;
	}
	return NULL;
}

/*
 * Unlinks and frees node from list at pos, returns pointer to data.
 *
 * 0 is front, -1 (or n - 1), both are done in O(1). Else O(n/2).
 */
void *list_delete(struct list *self, int pos)
{
	return list_node_unlink(self, list_index(self, pos));
}

/*
 * Pushes data to back of list in O(1). Returns new node.
 */
struct list_node *list_push_back(struct list *self, void *data)
{
	return list_insert(self, list_size(self), data);
}

/*
 * Pushes data to front of list in O(1). Returns new node.
 */
struct list_node *list_push_front(struct list *self, void *data)
{
	return list_insert(self, 0, data);
}

/*
 * Deletes tail node of list in O(1). Returns pointer to data.
 */
void *list_pop_back(struct list *self)
{
	return list_delete(self, -1);
}

/*
 * Deletes head node of list in O(1). Returns pointer to data.
 */
void *list_pop_front(struct list *self)
{
	return list_delete(self, 0);
}

/*
 * Returns pointer to data at tail of list in O(1).
 */
void *list_back(struct list *self)
{
	return list_tail(self)->data;
}

/*
 * Returns pointer to data at front of list in O(1).
 */
void *list_front(struct list *self)
{
	return list_head(self)->data;
}

/*
 * Returns pointer to head node of list in O(1).
 */
struct list_node *list_head(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_head(): self was null\n");
		return NULL;
	}

	return self->sentinel->next;
}

/*
 * Returns pointer to tail node of list in O(1).
 */
struct list_node *list_tail(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_tail(): self was null\n");
		return NULL;
	}

	return self->sentinel->prev;
}

/*
 * Returns node at pos in O(n/2).
 *
 * Iterates from the closest end. Supports negative pos arguments.
 */
struct list_node *list_index(struct list *self, int pos)
{
	int n = list_size(self);

	/* handle negative positions */
	if (pos < 0)
		pos += n;

	struct list_node *iter = NULL;

	if (pos >= n/2) {
		iter = list_head(self);
		for (int i = 0; i < pos; ++i)
			iter = iter->next;
	} else {
		iter = list_tail(self);
		for (int i = n - 1; i > pos; --i)
			iter = iter->prev;
	}

	return iter;
}

/*
 * Returns the number of nodes in list. Does not count the sentinel.
 */
size_t list_size(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_size(): self was null\n");
		return 0;
	}

	return self->size;
}

/*
 * Helper to check if size is 0.
 */
bool list_empty(struct list *self)
{
	return (list_size(self) == 0);
}

/*
 * Returns true if list_node was the sentiel.
 *
 * This is an indication that an iteration has reached the end of the
 * list. *Not* the last data-carrying node of the list.
 */
bool list_end(struct list_node *n)
{
	if (n == NULL) {
		fprintf(stderr, "list_end(): n was null\n");
		return false;
	}

	return n->sentinel;
}

/*
 * Use function to free data of each node, then free said node,
 * finally free the sentinel and the list.
 */
void list_free(struct list *self)
{
	while (!list_empty(self)) {
		void *d = list_pop_back(self);
		if (self->delete)
			self->delete(d);
	}

	free(self->sentinel);
	free(self);
}

/*
 * Default comparison for list of strings.
 */
bool list_default_compare(void *a, void *b)
{
	return (strcmp((char *)a, (char *)b) == 0);
}

/*
 * Allocates new list_node with data.
 *
 * Sentinel flag is false. The next and prev pointers are null.
 */
struct list_node *list_node_new(void *data)
{
	struct list_node *n = malloc(sizeof(*n));
	if (n == NULL) {
		perror("list_node_new()");
		return NULL;
	}

	n->sentinel = false;
	n->next = NULL;
	n->prev = NULL;
	n->data = data;

	return n;
}

/*
 * Given (a _ c), links b (new) leaving (a b c) in O(1).
 *
 * Node a is found from c, so with b and c as parameters, this
 * prepends (think cons).
 */
void list_node_link(struct list *self, struct list_node *b, struct list_node *c)
{
	if (self == NULL) {
		fprintf(stderr, "list_node_link(): self was null\n");
		return;
	}

	++self->size;
	struct list_node *a = c->prev;

	a->next = b;
	b->prev = a;
	b->next = c;
	c->prev = b;
}

/*
 * Given (a b c), unlinks b leaving (a _ c) in O(1).
 *
 * Nodes a and c are found from b. Yay double links.
 */
void *list_node_unlink(struct list *self, struct list_node *b)
{
	if (self == NULL) {
		fprintf(stderr, "list_node_unlink(): self was null\n");
		return NULL;
	}

	if (list_end(b))
		return NULL;

	--self->size;
	void *data = b->data;

	struct list_node *a = b->prev;
	struct list_node *c = b->next;

	a->next = c;
	c->prev = a;

	free(b);

	return data;
}
