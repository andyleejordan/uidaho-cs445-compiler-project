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
};

/* prepends token to list and returns pointer to new head of list */
struct tokenlist *tokenlist_prepend(struct token *token, struct tokenlist *list);

#endif /* TOKEN_H */
