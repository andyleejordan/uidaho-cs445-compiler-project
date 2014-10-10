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

#include "list.h"

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

struct list *list_new()
{
	struct list *self = malloc(sizeof(*self));
	if (self == NULL) {
		perror("list_new()");
		return NULL;
	}

	struct list_node *sentinel = list_node_new(NULL);
	if (sentinel == NULL) {
		free(self);
		return NULL;
	}

	self->sentinel = sentinel;
	self->size = 0;

	sentinel->sentinel = true;
	sentinel->next = sentinel;
	sentinel->prev = sentinel;

	return self;
}

/*
 * Use destroy function to free data of each node, free said node,
 * finally free sentinel and list.
 */
void list_free(struct list *self, void (*f)(void *data))
{
	while (!list_empty(self)) {
		void *d = list_pop(self);
		if (f != NULL)
			f(d);
	}

	free(self->sentinel);
	free(self);
}

size_t list_size(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_size(): self was null\n");
		return 0;
	}

	return self->size;
}

bool list_empty(struct list *self)
{
	return (list_size(self) == 0);
}

bool list_end(struct list_node *n)
{
	if (n == NULL) {
		fprintf(stderr, "list_end(): n was null\n");
		return false;
	}

	return n->sentinel;
}

struct list_node *list_head(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_head(): self was null\n");
		return NULL;
	}

	return self->sentinel->next;
}

struct list_node *list_tail(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_tail(): self was null\n");
		return NULL;
	}

	return self->sentinel->prev;
}

/*
 * Returns node at pos in O(n).
 *
 * Implemented like deque and iterates from the closest end.
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
 * Use compare function to return found node, else returns sentinel.
 */
struct list_node *list_find(struct list *self, void *data,
                            bool (*compare)(void *a, void *b)) {
	struct list_node *iter = list_head(self);
	while (!list_end(iter)) {
		if (compare(data, iter->data))
			return iter;
		iter = iter->next;
	}

	return iter;
}

bool list_contains(struct list *self, void *data,
                   bool (*compare)(void *a, void*b))
{
	return !list_end(list_find(self, data, compare));
}

/*
 * given (a _ c), links b (new) leaving (a b c)
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
 * Inserts data it at pos in O(n).
 *
 * Position 0 inserts at the front; n or -1 inserts at the end.
 */
struct list_node *list_insert(struct list *self, int pos, void *data)
{
	struct list_node *b = list_node_new(data);
	struct list_node *c = list_index(self, pos);

	list_node_link(self, b, c);

	return b;
}

/*
 * Pushes data to end of list in O(1).
 */
struct list_node *list_push(struct list *self, void *data)
{
	return list_insert(self, list_size(self), data);
}

/*
 * Pushes data to front of list in O(1).
 */
struct list_node *list_push_front(struct list *self, void *data)
{
	return list_insert(self, 0, data);
}

/*
 * given (a b c), unlinks b leaving (a _ c)
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

void *list_remove(struct list *self, int pos)
{
	return list_node_unlink(self, list_index(self, pos));
}

void *list_pop(struct list *self)
{
	return list_remove(self, -1);
}

void *list_pop_front(struct list *self)
{
	return list_remove(self, 0);
}

void *list_peek(struct list *self)
{
	return list_tail(self)->data;
}

void *list_peek_front(struct list *self)
{
	return list_head(self)->data;
}
