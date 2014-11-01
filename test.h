/*
 * test/test.h - Basic test framework interface.
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
#include <stdbool.h>

int status;

bool compare(const char *a, const char *b);
void running(const char *format, ...);
void testing(const char *format, ...);
void failure(const char *format, ...);

#endif /* TEST_H */
