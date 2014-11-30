/*
 * scope.h - Interface for scope handling.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef SCOPE_H
#define SCOPE_H

/* stack of scopes */
extern struct list *yyscopes;
#define scope_current() (struct hasht *)list_back(yyscopes)
#define scope_push(s) list_push_back(yyscopes, s)
#define scope_pop() list_pop_back(yyscopes)
struct typeinfo *scope_search(char *k);

#endif /* SCOPE_H */
