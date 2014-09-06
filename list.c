#include <stdlib.h>
#include <stdio.h>

#include "list.h"

struct list *list_init()
{
	struct list *self = malloc(sizeof(*self));
	struct list_node *sentinel = malloc(sizeof(*sentinel));
	if (self && sentinel)
		self->sentinel = sentinel;
	else
		goto error_malloc;

	sentinel->data.sentinel = true;
	sentinel->next = sentinel;
	sentinel->prev = sentinel;

	return self;

 error_malloc: {
		perror("list malloc");
		exit(EXIT_FAILURE);
	}
}

void list_prepend(struct list *self, union data data)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *temp = malloc(sizeof(*temp));
	if (temp == NULL)
		goto error_malloc;
	temp->data = data;

	temp->next = self->sentinel->next;
	self->sentinel->next->prev = temp;

	temp->prev = self->sentinel;
	self->sentinel->next = temp;

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

void list_append(struct list *self, union data data)
{
	if (self == NULL)
		goto error_null_self;

	struct list_node *temp = malloc(sizeof(*temp));
	if (temp == NULL)
		goto error_malloc;
	temp->data = data;

	temp->prev = self->sentinel->prev;
	self->sentinel->prev->next = temp;

	temp->next = self->sentinel;
	self->sentinel->prev = temp;

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
