#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

struct token
{
	int category;
	int lineno;
	char *text;
	char *filename;
	int ival;
	char *sval;
};

#endif /* TOKEN_H */
