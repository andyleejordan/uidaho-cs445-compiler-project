#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

#include "token.h"

/*
 * basic doubly linked circular list implementation
 * TODO add cleanup functions
 */

union data {
	bool sentinel;
	struct token *token;
	char *filename;
};

struct list_node
{
	struct list_node *next;
	struct list_node *prev;
	union data data;
};

struct list
{
	struct list_node *sentinel;
};

/* initializes head and tail of list; data fields to NULL */
struct list *list_init();
void list_destroy(struct list *self);
bool list_empty(struct list *self);
bool list_end(struct list_node *n);
struct list_node *list_head(struct list *self);
struct list_node *list_tail(struct list *self);
void list_push(struct list *self, union data data);
void list_push_front(struct list *self, union data data);
union data list_pop(struct list *self);
union data list_pop_front(struct list *self);
union data list_peek(struct list *self);
union data list_peek_front(struct list *self);

#endif /* LIST_H */
