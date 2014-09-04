#include <stdlib.h>

#include "token.h"

struct tokenlist *tokenlist_prepend(struct token *token, struct tokenlist *list)
{
  struct tokenlist *tmp = malloc(sizeof(*tmp));
  /* TODO if (tmp == NULL) something */
  tmp->data = token;
  tmp->next = list;
  return tmp;
}
