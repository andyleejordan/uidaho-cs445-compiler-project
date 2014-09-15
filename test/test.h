/*
 * test/test.h - Basic test framework.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef TEST_H
#define TEST_H

#include <stdlib.h>
#include <stdio.h>

int status = EXIT_SUCCESS;
char buffer[256];

void running(char *s)
{
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

#endif /* TEST_H */
