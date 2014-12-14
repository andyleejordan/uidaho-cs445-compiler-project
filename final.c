/*
 * final.c - Implementation of final code generation.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdio.h>

#include "type.h"

static char *map_region(enum region r);

void map_address(FILE *stream, struct address a)
{
	fprintf(stream, "(*(%s %s*)(%s + %d))",
	        print_basetype(a.type), a.type->pointer ? "*" : "",
	        map_region(a.region), a.offset);
}

void final_code(FILE *stream, struct list *code)
{
}

static char *map_region(enum region r)
{
	switch (r) {
	case GLOBE_R:
		return "global";
	case CONST_R:
		return "constant";
	default:
		return "unimplemented";
	}
}
