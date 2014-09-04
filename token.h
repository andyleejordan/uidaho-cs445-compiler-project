#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

typedef struct token
{
  int category;
  int lineno;
  char* text;
  char* filename;
  int ival;
  char* sval;
} token_t;

typedef struct tokenlist
{
  struct token* data;
  struct tokenlist* next;
} tokenlist_t;

/* prepends token to list and returns pointer to new head of list */
tokenlist_t* tokenlist_prepend(token_t*, tokenlist_t*);

#endif /* TOKEN_H */
