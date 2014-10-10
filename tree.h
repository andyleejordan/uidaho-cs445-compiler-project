/*
 * tree.h - List of lists with tokens.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef TREE_H
#define TREE_H

#include <stdlib.h>
#include <stdbool.h>

struct list;

struct tree {
	void *data;
	struct tree *parent;
	struct list *children;
};

struct tree *tree_new(struct tree *parent, void *data);
struct tree *tree_new_group(struct tree *parent, void *data, int count, ...);
void tree_free(struct tree *self, void (*f)(void *data, bool leaf));
size_t tree_size(struct tree *self);
struct tree *tree_push(struct tree *self, void *data);
void tree_preorder(struct tree *self, int d, void (*f)(struct tree *t, int d));

#endif /* TREE_H */
