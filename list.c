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
	if (self == NULL) {
		fprintf(stderr, "list_free(): self was null\n");
		return;
	}

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
	if (self == NULL) {
		fprintf(stderr, "list_empty(): self was null\n");
		return false;
	}

	return (self->size == 0);
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
		fprintf(stderr, "list_tail(): self was null\n");
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
 * Use compare function to return found node, else returns sentinel.
 */
struct list_node *list_find(struct list *self, void *data,
                            bool (*compare)(void *a, void *b)) {
	if (self == NULL) {
		fprintf(stderr, "list_find(): self was null\n");
		return NULL;
	}

	struct list_node *iter = list_head(self);
	while (!list_end(iter)) {
		if (compare(data, iter->data))
			return iter;
		iter = iter->next;
	}
	return iter;
}

/*
 * given (a c), links b leaving (a b c)
 */
void list_node_link(struct list_node *a, struct list_node *b)
{
	struct list_node *c = a->next;

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
	if (self == NULL) {
		fprintf(stderr, "list_insert(): self was null\n");
		return NULL;
	}

	struct list_node *n = list_node_new(data);

	struct list_node *iter = self->sentinel;
	if (pos > 0) {
		for (int i = 0; i < pos; ++i)
			iter = iter->next;
	} else if (pos < 0) {
		for (int i = 0; i > pos; --i)
			iter = iter->prev;
	}

	list_node_link(iter, n);
	++self->size;

	return n;
}

/*
 * Pushes data to end of list in O(1).
 */
struct list_node *list_push(struct list *self, void *data)
{
	struct list_node *n = list_insert(self, -1, data);
	return n;
}

/*
 * Pushes data to front of list in O(1);
 */
struct list_node *list_push_front(struct list *self, void *data)
{
	struct list_node *n = list_insert(self, 0, data);
	return n;
}

/*
 * given (a b c), unlinks b leaving (a c)
 */
void *list_node_unlink(struct list_node *b)
{
	void *data = b->data;

	struct list_node *a = b->prev;
	struct list_node *c = b->next;

	a->next = c;
	c->prev = a;

	return data;
}

void *list_pop(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_pop(): self was null\n");
		return NULL;
	}

	struct list_node *n = self->sentinel->prev;
	if (list_end(n)) {
		return NULL;
	}

	void *d = list_node_unlink(n);
	free(n);
	--self->size;

	return d;
}

void *list_pop_front(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_pop_front(): self was null\n");
		return NULL;
	}

	struct list_node *n = self->sentinel->next;
	if (list_end(n))
		return NULL;

	void *d = list_node_unlink(n);
	free(n);
	--self->size;

	return d;
}

void *list_peek(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_peek(): self was null\n");
		return NULL;
	}

	if (list_empty(self))
		return NULL;

	return self->sentinel->prev->data;
}

void *list_peek_front(struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_peek_front(): self was null\n");
		return NULL;
	}

	if (list_empty(self))
		return NULL;

	return self->sentinel->next->data;
}
