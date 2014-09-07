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
	sentinel->data.sentinel = true;
	sentinel->next = sentinel;
	sentinel->prev = sentinel;

	return self;

 error_malloc: {
		perror("list malloc");
		exit(EXIT_FAILURE);
	}
}

void list_push(struct list *self, union data data)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *n = malloc(sizeof(*n));
	if (n == NULL)
		goto error_malloc;
	n->data = data;

	n->prev = self->sentinel->prev;
	n->prev->next = n;

	n->next = self->sentinel;
	self->sentinel->prev = n;

	return;

 error_null_self: {
		fprintf(stderr, "list_append(): self was null\n");
		exit(EXIT_FAILURE);
	}
 error_malloc: {
		perror("list_append()");
		exit(EXIT_FAILURE);
	}
}

void list_push_front(struct list *self, union data data)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *n = malloc(sizeof(*n));
	if (n == NULL)
		goto error_malloc;
	n->data = data;

	n->next = self->sentinel->next;
	n->next->prev = n;

	n->prev = self->sentinel;
	self->sentinel->next = n;

	return;

 error_null_self: {
		fprintf(stderr, "list_prepend(): self was null\n");
		exit(EXIT_FAILURE);
	}
 error_malloc: {
		perror("list_prepend()");
		exit(EXIT_FAILURE);
	}
}

union data list_pop(struct list *self)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *n = self->sentinel->prev;
	union data d = n->data;

	self->sentinel->prev = n->prev;
	n->prev->next = self->sentinel;

	free(n);

	return d;

 error_null_self: {
		fprintf(stderr, "list_pop(): self was null\n");
		exit(EXIT_FAILURE);
	}
}

union data list_pop_front(struct list *self)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *n = self->sentinel->next;
	union data d = n->data;

	self->sentinel->next = n->next;
	n->next->prev = self->sentinel;

	free(n);

	return d;

 error_null_self: {
		fprintf(stderr, "list_pop_front(): self was null\n");
		exit(EXIT_FAILURE);
	}
}

union data list_peek(struct list *self)
{
	if (self == NULL)
		goto error_null_self;

	return self->sentinel->prev->data;

 error_null_self: {
		fprintf(stderr, "list_peek(): self was null\n");
		exit(EXIT_FAILURE);
	}
}

union data list_peek_front(struct list *self)
{
	if (self == NULL)
		goto error_null_self;

	return self->sentinel->next->data;

 error_null_self: {
		fprintf(stderr, "list_peek(): self was null\n");
		exit(EXIT_FAILURE);
	}
}
