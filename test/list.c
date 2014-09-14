/*
 * test/list.c - Unit test code for doubly linked list with sentinel.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../list.h"

void test_sentinel(struct list *list)
{
	struct list_node *sentinel = list->sentinel;

	if (!sentinel->sentinel)
		fprintf(stderr, "FAILURE sentinel should be marked as such\n");
}

void test_size(struct list *list, size_t size)
{
	if (list_size(list) != size)
		fprintf(stderr, "FAILURE list size should have been %lu\n", size);
}

void test_empty(struct list *list)
{
	test_size(list, 0);

	if (!list_empty(list))
		fprintf(stderr, "FAILURE empty list should have been empty\n");
}

void test_empty_sentinel(struct list *list)
{
	struct list_node *sentinel = list->sentinel;

	if (sentinel != list_head(list))
		fprintf(stderr, "FAILURE empty list sentinel should equal head\n");

	if (sentinel != list_tail(list))
		fprintf(stderr, "FAILURE empty list sentinel should equal tail\n");

	if (sentinel->next != sentinel)
		fprintf(stderr, "FAILURE empty list sentinel next should be itself\n");

	if (sentinel->prev != sentinel)
		fprintf(stderr, "FAILURE empty list sentinel prev should be itself\n");
}

void test_empty_ends(struct list *list)
{
	struct list_node *head = list_head(list);
	struct list_node *tail = list_tail(list);

	if (head != tail)
		fprintf(stderr, "FAILURE empty list head should equal tail\n");

	if (!list_end(head))
		fprintf(stderr, "FAILURE empty list head should have been end\n");
}

void test_non_empty(struct list *list)
{
	if (list_empty(list))
		fprintf(stderr, "FAILURE list should not have been empty\n");
}

void test_head_data(struct list *list, char *data)
{
	if (strcmp(list_peek_front(list), data) != 0)
		fprintf(stderr, "FAILURE list head should have been '%s'\n", data);
}

void test_tail_data(struct list *list, char *data)
{
	if (strcmp(list_peek(list), data) != 0)
		fprintf(stderr, "FAILURE list head should have been '%s'\n", data);
}

void test_iter_forward(struct list *list)
{
	struct list_node *iter = list_head(list);
	size_t counter = 0;
	while (!list_end(iter)) {
		++counter;
		printf("%s ", iter->data);
		iter = iter->next;
	}
	if (counter != list_size(list))
		fprintf(stderr, "FAILURE list iter forward counted %lu, but had size %lu\n", counter, list_size(list));
}

void test_iter_backward(struct list *list)
{
	struct list_node *iter = list_tail(list);
	size_t counter = 0;
	while (!list_end(iter)) {
		++counter;
		printf("%s ", iter->data);
		iter = iter->prev;
	}
	if (counter != list_size(list))
		fprintf(stderr, "FAILURE list iter backward counted %lu, but had size %lu\n", counter, list_size(list));
}

int main(int argc, char *argv[])
{
	printf("RUNNING list tests\n");

	printf("TESTING list initialization\n");
	struct list *list = list_init();
	assert(list != NULL);
	test_sentinel(list);
	test_empty(list);
	test_empty_sentinel(list);
	test_empty_ends(list);

	printf("TESTING list push\n");
	char *a = strdup("A");
	list_push(list, a);
	test_head_data(list, a);
	test_tail_data(list, a);

	char *b = strdup("B");
	list_push(list, b);
	test_tail_data(list, b);

	printf("TESTING list push front\n");
	list_push_front(list, b);
	test_head_data(list, b);

	printf("TESTING list not empty\n");
	test_non_empty(list);
	test_size(list, 3);

	printf("TESTING list pop\n");
	list_pop(list);
	test_tail_data(list, a);

	printf("TESTING list pop front\n");
	list_pop_front(list);
	test_head_data(list, a);

	printf("TESTING list emptied\n");
	list_pop(list);
	test_empty(list);

	printf("TESTING list forward iteration: ");
	list_push(list, a);
	list_push(list, b);
	test_iter_forward(list);
	printf("\n");

	printf("TESTING list backward iteration: ");
	test_iter_backward(list);
	printf("\n");

	list_destroy(list, &free);

	return 0;
}
