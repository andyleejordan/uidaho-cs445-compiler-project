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

#include <stddef.h>
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

struct list *list_new();

struct list_node *list_insert(struct list *self, int pos, void *data);
struct list_node *list_search(struct list *self, void *data,
                              bool (*compare)(void *a, void *b));
void *list_delete(struct list *self, int pos);

struct list_node *list_push_back(struct list *self, void *data);
struct list_node *list_push_front(struct list *self, void *data);

void *list_pop_back(struct list *self);
void *list_pop_front(struct list *self);

void *list_back(struct list *self);
void *list_front(struct list *self);

struct list_node *list_head(struct list *self);
struct list_node *list_tail(struct list *self);

struct list_node *list_index(struct list *self, int pos);
bool list_contains(struct list *self, void *data,
                   bool (*compare)(void *a, void*b));

size_t list_size(struct list *self);
bool list_empty(struct list *self);
bool list_end(struct list_node *n);

void list_free(struct list *self, void (*f)(void *data));

struct list_node *list_node_new(void *data);
void list_node_link(struct list *self, struct list_node *a, struct list_node *b);
void *list_node_unlink(struct list *self, struct list_node *b);

#endif /* LIST_H */
