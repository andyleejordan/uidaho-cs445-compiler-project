/*
 * include_string.h - Faux system header for <string>.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

class string {
public:
	string(char *);
	char *c_str();
private:
	char *data;
};

string::string(char *s)
{
	data = s;
}
