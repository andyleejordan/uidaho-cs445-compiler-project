#include <stdlib.h>

#include "token.h"

tokenlist_t* tokenlist_prepend(token_t* token, tokenlist_t* list)
{
  tokenlist_t* t = malloc(sizeof(tokenlist_t));
  t->data = token;
  t->next = list;
  return t;
}
