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

void list_prepend(struct list *self, union data data);
void list_append(struct list *self, union data data);

#endif /* LIST_H */
