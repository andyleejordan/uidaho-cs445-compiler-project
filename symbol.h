/*
 * symbol.h - Semantic data for symbols including type and scope.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "node.h"

struct tree;
struct hasht_node;
struct typeinfo;

void symbol_populate(struct tree *t);
void symbol_free(struct hasht_node *n);

struct typeinfo *type_check(struct tree *t);

struct tree *get_production(struct tree *n, enum rule r);
char *get_identifier(struct tree *n);
struct address get_address(struct tree *t);
bool get_pointer(struct tree *n);
int get_array(struct tree *n);
char *get_class(struct tree *n);
char *class_member(struct tree *n);

#endif /* SYMBOL_H */
