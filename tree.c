/*
 * tree.c - Implementation of tree.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tree.h"
#include "list.h"

/*
 * Initializes tree with reference to parent and data, and an empty
 * (but initialized) list of children.
 */
struct tree *tree_init(struct tree *parent, void *data)
{
	struct tree *t = malloc(sizeof(*t));
	if (t == NULL) {
		perror("tree_init()");
		return NULL;
	}

	struct list *l = list_init();
	if (l == NULL) {
		free(t);
		return NULL;
	}

	t->parent = parent;
	t->data = data;
	t->children = l;

	return t;
}

/*
 * Recursively calculates size of tree.
 */
size_t tree_size(struct tree *self)
{
	if (self == NULL)
		return 0;

	size_t size = 1;

	const struct list_node *iter = list_head(self->children);
	while (!list_end(iter)) {
		size += tree_size((struct tree *)iter->data);
		iter = iter->next;
	}

	return size;
}

/*
 * Basic pre-order print of data. Takes a function that converts the
 * data to its string representation (simplest form being a return for
 * char* types).
 */
void tree_print(struct tree *self, char *(*convert)(void *data))
{
	printf("(%s", convert(self->data));
	const struct list_node *iter = list_head(self->children);
	while (!list_end(iter)) {
		tree_print((struct tree *)iter->data, convert);
		iter = iter->next;
	}
	printf(")");
}

/*
 * Initializes a child tree with self as parent and data
 * reference. Pushes the child tree as data to self's list of
 * children. Returns reference to child tree.
 */
struct tree *tree_push(struct tree *self, void *data)
{
	struct tree *child = tree_init(self, data);
	if (child == NULL) {
		perror("tree_push()");
		return NULL;
	}

	list_push(self->children, child);

	return child;
}

void tree_destroy(struct tree *self, void (*destroy)(void *data))
{
	destroy(self->data);

	while (!list_empty(self->children)) {
		void *d = list_pop(self->children);
		tree_destroy(d, destroy);
	}

	free(self->children->sentinel);
	free(self->children);
}
