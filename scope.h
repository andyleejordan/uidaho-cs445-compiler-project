/*
 * scope.h - Interface for scope handling.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

#ifndef SCOPE_H
#define SCOPE_H

#include <stddef.h>

struct hasht;

/* stack of scopes */
extern struct list *yyscopes;

#define scope_current() (struct hasht *)list_back(yyscopes)
#define scope_constant() (struct hasht *)list_front(yyscopes)
#define scope_push(s) list_push_back(yyscopes, s)
#define scope_pop() list_pop_back(yyscopes)

struct typeinfo *scope_search(char *k);
size_t scope_size(struct hasht *t);

#endif /* SCOPE_H */
