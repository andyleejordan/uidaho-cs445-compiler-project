#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../../list.h"

/* smoke testing for list */

int main(int argc, char *argv[])
{
	struct list *list = list_init();

	if (!list_empty(list))
		fprintf(stderr, "list should have been empty\n");

	struct list_node *head = list_head(list);
	if (!list_end(head))
		fprintf(stderr, "head of empty list should have been end\n");

	char *a = calloc(2, sizeof(char));
	char *b = calloc(2, sizeof(char));
	char *c = calloc(2, sizeof(char));
	char *d = calloc(2, sizeof(char));

	strcpy(a, "A");
	strcpy(b, "B");
	strcpy(c, "C");
	strcpy(d, "D");

	list_push(list, (union data)b);
	list_push(list, (union data)c);
	list_push(list, (union data)d);
	list_push_front(list, (union data)a);
	list_push_front(list, (union data)d);
	list_pop_front(list);
	list_pop(list);
	free(d);

	if (list_empty(list))
		fprintf(stderr, "list should not have been empyty\n");

	if (list_size(list) != 3)
		fprintf(stderr, "list size should have been 3\n");

	if (strcmp(list_peek_front(list).filename, "A") != 0)
		fprintf(stderr, "list head should have been 'A'\n");

	if (strcmp(list_peek(list).filename, "C") != 0)
		fprintf(stderr, "list tail should have been 'C'\n");

	printf("forwards: ");
	struct list_node *iter = list_head(list);
	while (!list_end(iter)) {
		printf("%s ",iter->data.filename);
		iter = iter->next;
	}

	printf("/ backwards: ");
	iter = list_tail(list);
	while (!list_end(iter)) {
		printf("%s ", iter->data.filename);
		iter = iter->prev;
	}

	printf("\n");

	list_destroy(list, (void (*)(union data))&free);
	if (!list_empty(list))
		fprintf(stderr, "list should have been destroyed\n");

	return 0;
}
