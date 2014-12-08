/*
 * test/test.c - Implementation of test framework interface.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <string.h>
#include <stdarg.h>

#include "test.h"

bool compare(const char *a, const char *b)
{
	return (0 == strcmp(a, b));
}

void running(const char *format, ...)
{
	if (!TEST_DEBUG)
		return;

	va_list ap;
	va_start(ap, format);

	status = EXIT_SUCCESS;

	printf("RUNNING: ");
	vprintf(format, ap);
	printf(" tests\n");

	va_end(ap);
}

void testing(const char *format, ...)
{
	if (!TEST_DEBUG)
		return;

	va_list ap;
	va_start(ap, format);

	printf("TESTING: ");
	vprintf(format, ap);
	printf("\n");

	va_end(ap);
}

void failure(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);

	status = EXIT_FAILURE;
	fprintf(stderr, "FAILURE: ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "!\n");

	va_end(ap);
}
