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

enum opcode {
	/* pseudo opcodes */
	GLOBAL, /* name, address in GLOBAL_R */
	PROC,   /* name, address in GLOBAL_R */
	LOCAL,  /* name, address in LOCAL_R */
	LABEL,  /* name (optional), address in LABEL_R */
	END,    /* declares end of current procedure */
	/* actual opcodes */
	ADD,    /* x := y op z  store result of binary operation on y and z to x */
	SUB,
	MUL,
	DIV,
	MOD,
	NEG,    /* x := op y    store result of unary operation on y to x */
	ASN,    /* x := y       store y to x */
	ADDR,   /* x := &y      store address of y to x */
	LCONT,  /* x := *y      store contents pointed to by y to x */
	SCONT,  /* *x := y      store y to location pointed to by x */
	GOTO,   /* goto L       unconditional jump to L */
	BLT,    /* if x rop y then goto L  binary conditional jump to L */
	BLE,
	BGT,
	BGE,
	BEQ,
	BNE,
	BIF,    /* if x then goto L   unary conditional jump to L */
	BNIF,   /* if !x then goto L  unary negative conditional jump to L */
	PARAM,  /* param x        store x as a parameter */
	CALL,   /* call p, x, n   call procedure p with n parameters, store result in x */
	RET,    /* return x       return from procedure, use x as the result */
	ERRC,
};

struct op {
	enum opcode code;
	char *name;
	struct address address[3];
};

void code_generate(struct tree *t);
void print_code(FILE *stream, struct list *code);

#endif /* INTERMEDIATE_H */
