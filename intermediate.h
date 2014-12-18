/*
 * intermediate.h - Intermediate code generation.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include <stdio.h>

#include "node.h"
#include "type.h"

enum opcode {
	/* pseudo opcodes */
	PROC_O,   /* name, bytes of parameters, bytes of locals, in GLOBAL_R,  */
	END_O,    /* declares end of current procedure */
	PARAM_O,  /* param x          store x as a parameter */
	CALL_O,   /* call p, x, n     call procedure p with n parameters, store result in x */
	RET_O,    /* return x         return from procedure, use x as the result */
	LABEL_O,  /* name (optional), in LABEL_R */
	GOTO_O,   /* goto L           unconditional jump to L */
	NEW_O,    /* x := new Foo, n  create a new instance of class Foo of size n */
	DEL_O,    /* delete object    free memory allocated for object */
	/* psudeo opcodes for printing types with cout << thing */
	PINT_O,
	PCHAR_O,
	PBOOL_O,
	PFLOAT_O,
	PSTR_O,
	/* actual opcodes */
	ADD_O,    /* x := y op z  store result of binary operation on y and z to x */
	FADD_O,   /* floating point versions */
	SUB_O,
	FSUB_O,
	MUL_O,
	FMUL_O,
	DIV_O,
	FDIV_O,
	MOD_O,
	LT_O,    /* x < y */
	FLT_O,
	LE_O,    /* x <= y */
	FLE_O,
	GT_O,    /* x > y */
	FGT_O,
	GE_O,    /* x >= y */
	FGE_O,
	EQ_O,    /* x == y */
	FEQ_O,
	NE_O,    /* x != y */
	FNE_O,
	OR_O,    /* x || y */
	AND_O,   /* x && y */
	NEG_O,    /* x := -y      store result of integer neg operation on y to x */
	FNEG_O,
	NOT_O,    /* x := !y      store result of logical not operation on y to x */
	LSTAR_O,  /* *x := y      store y to location pointed to by x */
	RSTAR_O,  /* x := *y      store contents pointed to by y to x */
	ADDR_O,   /* x := &y      store address of y to x */
	ASN_O,    /* x := y       store y to x */
	LARR_O,   /* array[x] = y store y at array index x */
	RARR_O,   /* x = array[y] retrieve array element at index y */
	LFIELD_O, /* class.field = x */
	RFIELD_O, /* x = class.field */
	IF_O,     /* if x then goto L  unary conditional jump to L */
	ERRC_O,
};

struct op {
	enum opcode code;
	char *name;
	struct address address[3];
};

void code_generate(struct tree *t);
void print_op(FILE *stream, struct op *op);
void print_code(FILE *stream, struct list *code);

#endif /* INTERMEDIATE_H */
