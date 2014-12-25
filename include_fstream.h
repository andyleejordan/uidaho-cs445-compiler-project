/*
 * include_fstream.h - Faux system header for <fstream>.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

class ifstream {
public:
	bool is_open();
	void open(char *);
	void close();
	void ignore();
	bool eof();
};

class ofstream {
public:
	bool is_open();
	void open(char *);
	void close();
	bool eof();
};
