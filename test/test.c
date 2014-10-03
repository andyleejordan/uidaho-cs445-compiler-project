/*
 * test/test.c - Implementation of test framework interface.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <string.h>

#include "test.h"

bool compare(const char *a, const char *b) {
	return (0 == strcmp(a, b));
}

void running(char *s)
{
	status = EXIT_SUCCESS;
	printf("RUNNING %s tests\n", s);
}

void testing(char *s)
{
	printf("TESTING %s\n", s);
}

void failure(char *s)
{
	status = EXIT_FAILURE;
	fprintf(stderr, "FAILURE %s!\n", s);
}
