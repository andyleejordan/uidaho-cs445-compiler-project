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

int status;
char buffer[256];

void running(char *s);
void testing(char *s);
void failure(char *s);

#endif /* TEST_H */
