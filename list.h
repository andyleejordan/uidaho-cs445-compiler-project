/*
 * list.h - Interface for doubly linked circular list.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

#include "token.h"

union data {
	void *empty;
	struct token *token;
	char *filename;
};

struct list_node
{
	struct list_node *next;
	struct list_node *prev;
	bool sentinel;
	union data data;
};

struct list
{
	struct list_node *sentinel;
	size_t size;
};

struct list *list_init();
void list_destroy(struct list *self, void (*destroy)(union data));
size_t list_size(const struct list *self);
bool list_empty(const struct list *self);
bool list_end(const struct list_node *n);
struct list_node *list_head(const struct list *self);
struct list_node *list_tail(const struct list *self);
void list_push(struct list *self, union data data);
void list_push_front(struct list *self, union data data);
union data list_pop(struct list *self);
union data list_pop_front(struct list *self);
union data list_peek(const struct list *self);
union data list_peek_front(const struct list *self);

#endif /* LIST_H */
