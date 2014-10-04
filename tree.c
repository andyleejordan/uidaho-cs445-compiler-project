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
#include <stdarg.h>

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
 * Initializes tree with reference to parent and data, and pushes
 * count number of following 'void *' arguments to tree as children.
 */
struct tree *tree_initv(struct tree *parent, void *data, int count, ...)
{
	va_list ap;
	va_start(ap, count);

	struct tree *t = tree_init(parent, data);

	for (int i = 0; i < count; ++i)
		tree_push(t, va_arg(ap, void *));

	va_end(ap);
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
 * Pre-order traversal of tree. Takes a function and applies it to
 * each subtree.
 */
void tree_preorder(struct tree *self, void (*f)(struct tree *t))
{
	f(self);
	const struct list_node *iter = list_head(self->children);
	while (!list_end(iter)) {
		tree_preorder((struct tree *)iter->data, f);
		iter = iter->next;
	}
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

/*
 * Recursively deallocates a tree, and optionally frees data if given
 * a non-NULL function pointer.
 */
void tree_destroy(struct tree *self, void (*destroy)(void *data))
{
	if (destroy != NULL)
		destroy(self->data);

	while (!list_empty(self->children)) {
		void *d = list_pop(self->children);
		tree_destroy(d, destroy);
	}

	free(self->children->sentinel);
	free(self->children);
}
