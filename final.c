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

#include "list.h"
#include "hasht.h"

#define p(...) fprintf(stream, __VA_ARGS__)

extern struct list *yyscopes;

static char *map_region(enum region r);
static void map_address(FILE *stream, struct address a);

void final_code(FILE *stream, struct list *code)
{
	p("int main()\n{\n");
	p("\t/* initializing constant region */\n");
	struct hasht *constant = list_front(yyscopes);
	for (size_t i = 0; i < constant->size; ++i) {
		struct hasht_node *slot = constant->table[i];
		if (slot && !hasht_node_deleted(slot)) {
			struct typeinfo *v = slot->value;
			if (v->base == FLOAT_T || (v->base == CHAR_T && v->pointer)) {
				p("\t");
				map_address(stream, v->place);
				p(" = %s;\n", slot->key);
			}
		}
	}
	fprintf(stream, "}");

}

/* returns code to get value at address */
static void map_address(FILE *stream, struct address a)
{
	/* if an immediate, use it */
	if (a.region == CONST_R && !a.type->pointer
	    && (a.type->base == INT_T
	        || a.type->base == CHAR_T
	        || a.type->base == BOOL_T))
		p("(%d)", a.offset);
	/* otherwise grab from region */
	else
		p("(*(%s %s*)(%s + %d%s))",
		  print_basetype(a.type), a.type->pointer ? "*" : "",
		  map_region(a.region), a.offset,
		  a.region == LOCAL_R ? " + param" : "");
}

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

#undef p
