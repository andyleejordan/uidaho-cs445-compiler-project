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

struct tree *tree_init(struct tree *parent, void *data);
struct tree *tree_initv(struct tree *parent, void *data, int count, ...);
void tree_destroy(struct tree *self, void (*destroy)(void *data, bool leaf));
size_t tree_size(struct tree *self);
void tree_preorder(struct tree *self, int depth, void (*f)(struct tree *t, int d));
struct tree *tree_push(struct tree *self, void *data);

#endif /* TREE_H */
