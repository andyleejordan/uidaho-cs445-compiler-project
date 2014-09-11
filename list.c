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

struct list_node *list_node_init()
{
	struct list_node *n = malloc(sizeof(*n));
	if (n == NULL) {
		perror("list_node_init()");
		return NULL;
	}

	n->sentinel = false;
	n->next = NULL;
	n->prev = NULL;

	return n;
}

struct list *list_init()
{
	struct list *self = malloc(sizeof(*self));
	if (self == NULL) {
		perror("list_init()");
		return NULL;
	}

	struct list_node *sentinel = list_node_init();
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

void list_destroy(struct list *self, void (*destroy)(union data))
{
	if (self == NULL) {
		fprintf(stderr, "list_destroy(): self was null\n");
		return;
	}

	while (!list_empty(self)) {
		union data d = list_pop(self);
		if (d.token != NULL)
			destroy((union data)d.token);
		else if (d.filename != NULL)
			destroy((union data)d.filename);
	}
}

size_t list_size(const struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_size(): self was null\n");
		return 0;
	}

	return self->size;
}

bool list_empty(const struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_empty(): self was null\n");
		return false;
	}

	return (self->size == 0);
}

bool list_end(const struct list_node *n)
{
	if (n == NULL) {
		fprintf(stderr, "list_end(): n was null\n");
		return false;
	}

	return n->sentinel;
}

struct list_node *list_head(const struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_tail(): self was null\n");
		return NULL;
	}

	return self->sentinel->next;
}

struct list_node *list_tail(const struct list *self)
{
	if (self == NULL) {
		fprintf(stderr, "list_tail(): self was null\n");
		return NULL;
	}

	return self->sentinel->prev;
}

void list_push(struct list *self, union data data)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *n = list_node_init();
	if (n == NULL)
		goto error_malloc;
	n->data = data;

	n->prev = self->sentinel->prev;
	n->prev->next = n;

	n->next = self->sentinel;
	self->sentinel->prev = n;

	++self->size;

	return;

 error_null_self:
	fprintf(stderr, "list_push(): self was null\n");
	exit(EXIT_FAILURE);

 error_malloc:
	perror("list_push()");
	exit(EXIT_FAILURE);
}

void list_push_front(struct list *self, union data data)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *n = list_node_init();
	if (n == NULL)
		goto error_malloc;
	n->data = data;

	n->next = self->sentinel->next;
	n->next->prev = n;

	n->prev = self->sentinel;
	self->sentinel->next = n;

	++self->size;

	return;

 error_null_self:
	fprintf(stderr, "list_push_front(): self was null\n");
	exit(EXIT_FAILURE);

 error_malloc:
	perror("list_push_front()");
	exit(EXIT_FAILURE);
}

union data list_pop(struct list *self)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *n = self->sentinel->prev;
	union data d = n->data;

	self->sentinel->prev = n->prev;
	n->prev->next = self->sentinel;

	if (!list_end(n))
		free(n);

	--self->size;

	return d;

 error_null_self:
	fprintf(stderr, "list_pop(): self was null\n");
	exit(EXIT_FAILURE);
}

union data list_pop_front(struct list *self)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *n = self->sentinel->next;
	union data d = n->data;

	self->sentinel->next = n->next;
	n->next->prev = self->sentinel;

	if (!list_end(n))
		free(n);

	--self->size;

	return d;

 error_null_self:
	fprintf(stderr, "list_pop_front(): self was null\n");
	exit(EXIT_FAILURE);
}

union data list_peek(const struct list *self)
{
	if (self == NULL)
		goto error_null_self;

	if (list_empty(self))
		return (union data)NULL;

	return self->sentinel->prev->data;

 error_null_self:
	fprintf(stderr, "list_peek(): self was null\n");
	exit(EXIT_FAILURE);
}

union data list_peek_front(const struct list *self)
{
	if (self == NULL)
		goto error_null_self;

	if (list_empty(self))
		return (union data)NULL;

	return self->sentinel->next->data;

 error_null_self:
	fprintf(stderr, "list_peek(): self was null\n");
	exit(EXIT_FAILURE);
}
