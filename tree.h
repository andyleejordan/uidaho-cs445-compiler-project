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

#include <stddef.h>
#include <stdbool.h>

struct list;

struct tree {
	void *data;
	struct tree *parent;
	struct list *children;
	bool (*compare)(void *a, void *b);
	void (*delete)(void *data, bool leaf);
};

struct tree *tree_new(struct tree *parent, void *data,
                      bool (*compare)(void *a, void *b),
                      void (*delete)(void *data, bool leaf));
struct tree *tree_new_group(struct tree *parent, void *data,
                            bool (*compare)(void *a, void *b),
                            void (*delete)(void *data, bool leaf),
                            int count, ...);

struct tree *tree_push_front(struct tree *self, void *data);
struct tree *tree_push_back(struct tree *self, void *data);
struct tree *tree_push_child(struct tree *self, struct tree *child);

struct tree *tree_index(struct tree *self, int pos);

void tree_traverse(struct tree *self, int d,
                   bool (*pre) (struct tree *t, int d),
                   void (*in)  (struct tree *t, int d),
                   void (*post)(struct tree *t, int d));

size_t tree_size(struct tree *self);
void tree_free(struct tree *self);

#endif /* TREE_H */
