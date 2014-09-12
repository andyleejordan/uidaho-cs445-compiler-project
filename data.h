/*
 * data.h - Union representing an abstract container.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef DATA_H
#define DATA_H

struct token;

union data {
	void *empty;
	struct token *token;
	char *filename;
};

#endif /* DATA_H */
