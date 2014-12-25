/*
 * libs.h - Supports handling of standard libraries.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

#ifndef LIBS_H
#define LIBS_H

#include <stdbool.h>

struct libs {
	bool usingstd;
	bool cstdlib;
	bool cmath;
	bool ctime;
	bool cstring;
	bool fstream;
	bool iostream;
	bool string;
	bool iomanip;
} libs;

#endif /* LIBS_H */
