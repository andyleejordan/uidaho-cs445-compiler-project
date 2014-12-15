/*
 * final.c - Implementation of final code generation.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <stdio.h>
#include <string.h>

#include "intermediate.h"
#include "type.h"

#include "list.h"
#include "hasht.h"

#define p(...) fprintf(stream, __VA_ARGS__)

extern struct list *yyscopes;

static void map_instruction(FILE *stream, struct op *op);
static char *map_op(enum opcode code);
static char *map_print(enum opcode code);
static char *map_region(enum region r);
static void map_address(FILE *stream, struct address a);

void final_code(FILE *stream, struct list *code)
{
	struct hasht *global = list_index(yyscopes, 1)->data;
	for (size_t i = 0; i < global->size; ++i) {
		struct hasht_node *slot = global->table[i];
		if (slot && !hasht_node_deleted(slot)) {
			struct typeinfo *value = slot->value;
			if (value->base == FUNCTION_T) {
				if (strcmp(slot->key, "main") == 0)
					continue;
				p("%s %s(int);\n",
				  print_basetype(value->function.type), slot->key);
			}
		}
	}

	p("void _initialize_constants()\n{\n");
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
	p("}\n");

	/* generate C instructions for list of TAC ops */
	struct list_node *iter = list_head(code);
	while (!list_end(iter)) {
		map_instruction(stream, iter->data);
		iter = iter->next;
	}
}

static void map_instruction(FILE *stream, struct op *op)
{
	p("/*\n");
	print_op(stream, op);
	p("*/\n");
	static int param_offset = 0;
	struct address a = op->address[0];
	struct address b = op->address[1];
	struct address c = op->address[2];
	switch (op->code) {
	case PROC_O:
		p("%s %s(int n)\n{\n",
		  print_basetype(c.type->function.type), op->name);
		p("\tint param = %d;\n", a.offset);
		p("\tchar local[%d];\n", b.offset);
		if (strcmp(op->name, "main") == 0)
			p("\t_initialize_constants();\n");
		/* copy parameters from faux stack into front of local region */
		p("\tmemcpy(local, stack, param); /* copy parameters */\n");
		break;
	case END_O:
		p("}\n");
		break;
	case PARAM_O:
		/* save parameters into faux stack */
		p("(*(%s %s*)(stack + %d))",
		  print_basetype(a.type), a.type->pointer ? "*" : "", param_offset);
		p(" = ");
		map_address(stream, a);
		p(";\n");
		param_offset += typeinfo_size(a.type);
		break;
	case CALL_O:
		/* reset parameter offset and call function, saving return */
		param_offset = 0;
		p("\t");
		if (!(a.type->base == VOID_T && !a.type->pointer)) {
			map_address(stream, a);
			p(" = ");
		}
		p("%s(%d);\n", op->name, b.offset);
		break;
	case RET_O:
		p("\treturn");
		if (!(a.type->base == VOID_T && !a.type->pointer))
			map_address(stream, a);
		p(";\n");
		break;
	case LABEL_O:
		p("L_%d:\n", a.offset);
		break;
	case GOTO_O:
		p("\tgoto L_%d;\n", a.offset);
		break;
	case PINT_O:
	case PCHAR_O:
	case PBOOL_O:
	case PFLOAT_O:
	case PSTR_O:
		p("\tprintf(\"%s\", ", map_print(op->code));
		map_address(stream, a);
		p(");\n");
		break;
	case ADD_O:
	case FADD_O:
	case SUB_O:
	case FSUB_O:
	case MUL_O:
	case FMUL_O:
	case DIV_O:
	case FDIV_O:
	case MOD_O:
	case LT_O:
	case FLT_O:
	case LE_O:
	case FLE_O:
	case GT_O:
	case FGT_O:
	case GE_O:
	case FGE_O:
	case EQ_O:
	case FEQ_O:
	case NE_O:
	case FNE_O:
	case OR_O:
	case AND_O:
		p("\t");
		map_address(stream, a);
		p(" = ");
		map_address(stream, b);
		p(" %s ", map_op(op->code));
		map_address(stream, c);
		p(";\n");
		break;
	case NEG_O:
	case FNEG_O:
	case NOT_O:
	case ASN_O:
	case LCONT_O:
		p("\t");
		map_address(stream, a);
		p(" = ");
		p("%s", map_op(op->code));
		map_address(stream, b);
		p(";\n");
		break;
	case SCONT_O:
		p("\t");
		p("(**(%s %s*)(%s + %d%s))",
		  print_basetype(a.type), a.type->pointer ? "*" : "",
		  map_region(a.region), a.offset,
		  a.region == LOCAL_R ? " + param" : "");
		p(" = ");
		map_address(stream, b);
		p(";\n");
		break;
	case ADDR_O:
		p("\t");
		map_address(stream, a);
		p(" = ");
		p("((%s %s*)(%s + %d%s))",
		  print_basetype(b.type), b.type->pointer ? "*" : "",
		  map_region(b.region), b.offset,
		  b.region == LOCAL_R ? " + param" : "");
		p(";\n");
		break;
	case ARR_O:
		p("\t");
		p("(*(%s %s**)(%s + %d%s))",
		  print_basetype(a.type), a.type->pointer ? "*" : "",
		  map_region(a.region), a.offset,
		  a.region == LOCAL_R ? " + param" : "");
		p(" = ");
		p("((%s %s*)(%s + %d%s + ",
		  print_basetype(b.type), b.type->pointer ? "*" : "",
		  map_region(b.region), b.offset,
		  b.region == LOCAL_R ? " + param" : "");
		map_address(stream, c);
		p("));\n");
		break;
	default:
		break;
	}
}

/* returns code to get value at address */
static void map_address(FILE *stream, struct address a)
{
	if (a.region == UNKNOWN_R)
		return;
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

/* returns string representation of binary operator */
static char *map_op(enum opcode code)
{
	switch (code) {
	case ADD_O:
	case FADD_O:
		return "+";
	case SUB_O:
	case FSUB_O:
		return "-";
	case MUL_O:
	case FMUL_O:
		return "*";
	case DIV_O:
	case FDIV_O:
		return "/";
	case MOD_O:
		return "%";
	case LT_O:
	case FLT_O:
		return "<";
	case LE_O:
	case FLE_O:
		return "<=";
	case GT_O:
	case FGT_O:
		return ">";
	case GE_O:
	case FGE_O:
		return ">=";
	case EQ_O:
	case FEQ_O:
		return "==";
	case NE_O:
	case FNE_O:
		return "!=";
	case OR_O:
		return "||";
	case AND_O:
		return "&&";
	case NEG_O:
	case FNEG_O:
		return "-";
	case NOT_O:
		return "!";
	case ASN_O:
		return "";
	case ADDR_O:
		return "&";
	case LCONT_O:
		return "*";
	default:
		return NULL;
	}
}

/* returns conversion specifier string */
static char *map_print(enum opcode code)
{
	switch (code) {
	case PINT_O:
	case PBOOL_O:
		return "%d";
	case PCHAR_O:
		return "%c";
	case PFLOAT_O:
		return "%f";
	case PSTR_O:
		return "%s";
	default:
		return NULL;
	}
}

/* returns name of region variable in generated code */
static char *map_region(enum region r)
{
	switch (r) {
	case GLOBE_R:
		return "global";
	case CONST_R:
		return "constant";
	case LOCAL_R:
	case PARAM_R:
		return "local";
	default:
		return "unimplemented";
	}
}

#undef p
