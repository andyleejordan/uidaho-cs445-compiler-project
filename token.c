#include <stdlib.h>

#include "token.h"

void tokenlist_init(struct tokenlist **head, struct tokenlist **tail)
{
	*head = malloc(sizeof(**head));
	*tail = *head;
	(*head)->data = NULL;
	(*head)->next = *head;
	(*head)->prev = *head;
}

void tokenlist_prepend(struct token *token, struct tokenlist **head)
{
	struct tokenlist *temp = malloc(sizeof(*temp));
	/* TODO if (tmp == NULL) error */
	temp->data = token;

	struct tokenlist *sent = (*head)->prev;

	temp->prev = sent;
	sent->next = temp;

	temp->next = *head;
	(*head)->prev = temp;

	*head = temp;
}

void tokenlist_append(struct token *token, struct tokenlist **tail)
{
	struct tokenlist *temp = malloc(sizeof(*temp));
	/* TODO if (tmp == NULL) error */
	temp->data = token;

	struct tokenlist *sent = (*tail)->next;

	temp->next = sent;
	temp->prev = *tail;

	sent->prev = temp;
	(*tail)->next = temp;

	*tail = temp;
}
