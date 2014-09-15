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

#include "test.h"
#include "../list.h"

void test_sentinel(struct list *list)
{
	struct list_node *sentinel = list->sentinel;

	if (!sentinel->sentinel)
		failure("sentinel should be marked as such");
}

void test_size(struct list *list, size_t size)
{
	if (list_size(list) != size) {
		sprintf(buffer, "size should have been %lu", size);
		failure(buffer);
	}
}

void test_empty(struct list *list)
{
	test_size(list, 0);

	if (!list_empty(list))
		failure("empty list should have been empty");
}

void test_empty_sentinel(struct list *list)
{
	struct list_node *sentinel = list->sentinel;

	if (sentinel != list_head(list))
		failure("empty list sentinel should equal head");

	if (sentinel != list_tail(list))
		failure("empty list sentinel should equal tail");

	if (sentinel->next != sentinel)
		failure("empty list sentinel next should be itself");

	if (sentinel->prev != sentinel)
		failure("empty list sentinel prev should be itself");
}

void test_empty_ends(struct list *list)
{
	struct list_node *head = list_head(list);
	struct list_node *tail = list_tail(list);

	if (head != tail)
		failure("empty list head should equal tail");

	if (!list_end(head))
		failure("empty list head should have been end");
}

void test_non_empty(struct list *list)
{
	if (list_empty(list))
		failure("should not have been empty");
}

void test_head_data(struct list *list, char *data)
{
	if (strcmp(list_peek_front(list), data) != 0) {
		sprintf(buffer, "head should have been '%s'", data);
		failure(buffer);
	}
}

void test_tail_data(struct list *list, char *data)
{
	if (strcmp(list_peek(list), data) != 0) {
		sprintf(buffer, "head should have been '%s'", data);
		failure(buffer);
	}
}

void test_iter_forward(struct list *list)
{
	struct list_node *iter = list_head(list);
	size_t counter = 0;
	while (!list_end(iter)) {
		++counter;
		printf("%s ", (char *)iter->data);
		iter = iter->next;
	}
	if (counter != list_size(list)) {
		sprintf(buffer, "iter forward counted %lu, but had size %lu", counter, list_size(list));
		failure(buffer);
	}
}

void test_iter_backward(struct list *list)
{
	struct list_node *iter = list_tail(list);
	size_t counter = 0;
	while (!list_end(iter)) {
		++counter;
		printf("%s ", (char *)iter->data);
		iter = iter->prev;
	}
	if (counter != list_size(list)) {
		sprintf(buffer, "iter backward counted %lu, but had size %lu", counter, list_size(list));
		failure(buffer);
	}
}

int main(int argc, char *argv[])
{
	running("list");

	testing("initialization");
	struct list *list = list_init();
	assert(list != NULL);
	test_sentinel(list);
	test_empty(list);
	test_empty_sentinel(list);
	test_empty_ends(list);

	testing("push");
	char *a = strdup("A");
	list_push(list, a);
	test_head_data(list, a);
	test_tail_data(list, a);

	char *b = strdup("B");
	list_push(list, b);
	test_tail_data(list, b);

	testing("push front");
	list_push_front(list, b);
	test_head_data(list, b);

	testing("not empty");
	test_non_empty(list);
	test_size(list, 3);

	testing("pop");
	list_pop(list);
	test_tail_data(list, a);

	testing("pop front");
	list_pop_front(list);
	test_head_data(list, a);

	testing("emptied");
	list_pop(list);
	test_empty(list);

	testing("forward iteration:");
	list_push(list, a);
	list_push(list, b);
	test_iter_forward(list);
	printf("\n");

	testing("backward iteration:");
	test_iter_backward(list);
	printf("\n");

	list_destroy(list, &free);

	return status;
}
