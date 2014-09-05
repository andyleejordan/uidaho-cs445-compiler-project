#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

struct token
{
	int category;
	int lineno;
	char *text;
	char *filename;
	int ival;
	char *sval;
};

struct tokenlist
{
	struct token *data;
	struct tokenlist *next;
	struct tokenlist *prev;
};

/* initializes head and tail of tokenlist; data fields to NULL */
void tokenlist_init(struct tokenlist **head, struct tokenlist **tail);

/* prepends token to head of list and returns pointer to new head */
void tokenlist_prepend(struct token *token, struct tokenlist **head);

/* appends token to tail of list and returns pointer to new tail */
void tokenlist_append(struct token *token, struct tokenlist **tail);

#endif /* TOKEN_H */
