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

#include <stdlib.h>
#include <stdbool.h>

struct list_node
{
	struct list_node *next;
	struct list_node *prev;
	bool sentinel;
	void *data;
};

struct list
{
	struct list_node *sentinel;
	size_t size;
};

struct list *list_init();
void list_destroy(struct list *self, void (*destroy)(void *data));
size_t list_size(const struct list *self);
bool list_empty(const struct list *self);
bool list_end(const struct list_node *n);
struct list_node *list_head(const struct list *self);
struct list_node *list_tail(const struct list *self);
struct list *list_push(struct list *self, void *data);
struct list *list_push_front(struct list *self, void *data);
void *list_pop(struct list *self);
void *list_pop_front(struct list *self);
void *list_peek(const struct list *self);
void *list_peek_front(const struct list *self);

#endif /* LIST_H */
