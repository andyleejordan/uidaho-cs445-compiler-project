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

struct list *list_init()
{
	struct list *self = malloc(sizeof(*self));
	struct list_node *sentinel = malloc(sizeof(*sentinel));
	if (self == NULL || sentinel == NULL)
		goto error_malloc;

	self->sentinel = sentinel;
	self->size = 0;

	sentinel->sentinel = true;
	sentinel->next = sentinel;
	sentinel->prev = sentinel;

	return self;

 error_malloc:
	perror("list malloc");
	exit(EXIT_FAILURE);
}

void list_destroy(struct list *self, void (*destroy)(union data))
{
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
	return self->size;
}

bool list_empty(const struct list *self)
{
	return (self->size == 0);
}

bool list_end(const struct list_node *n)
{
	return n->sentinel;
}

struct list_node *list_head(const struct list *self)
{
	return self->sentinel->next;
}

struct list_node *list_tail(const struct list *self)
{
	return self->sentinel->prev;
}

void list_push(struct list *self, union data data)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *n = malloc(sizeof(*n));
	if (n == NULL)
		goto error_malloc;
	n->sentinel = false;
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

	struct list_node *n = malloc(sizeof(*n));
	if (n == NULL)
		goto error_malloc;
	n->sentinel = false;
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
