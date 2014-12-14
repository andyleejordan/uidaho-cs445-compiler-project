/*
 * final.h - TAC-C final code generation.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef FINAL_H
#define FINAL_H

#include <stddef.h>
#include <stdio.h>

struct list;

void final_code(FILE *stream, struct list *code);

#endif /* FINAL_H */
