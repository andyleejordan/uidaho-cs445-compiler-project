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

#include "node.h"

enum opcode {
	ADD,    /* x := y op z  store result of binary operation on y and z to x */
	SUB,
	MUL,
	DIV,
	NEG,    /* x := op y      store result of unary operation on y to x */
	ASN,    /* x := y store y to x */
	ADDR,   /* x := &y        store address of y to x */
	LCONT,  /* x := *y        store contents pointed to by y to x */
	SCONT,  /* *x := y        store y to location pointed to by x */
	GOTO,   /* goto L unconditional jump to L */
	BLT,    /* if x rop y then goto L binary conditional jump to L */
	BLE,
	BGT,
	BGE,
	BEQ,
	BNE,
	BIF,    /* if x then goto L             unary conditional jump to L */
	BNIF,   /* if !x then goto L              unary negative conditional jump to L */
	PARM,   /* param x        store x as a parameter */
	CALL,   /* call p, n, x   call procedure p with n parameters, store result in x */
	RET     /* return x       return from procedure, use x as the result */
};

struct op {
	enum opcode code;
	struct address address[3];
};

void code_generate();

#endif /* INTERMEDIATE_H */
