/*
 * math.cpp - Valid C math.h code for testing compiler.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

/* Note that this does not pull in prototypes */
#include <math.h>
double fmin(double, double);

/* Note that variadic functions, and so all of stdio.h, are not supported! */
#include <iostream>
using namespace std;


int main(int argc, char *argv[])
{
	double pi = 3.141592653;
	double tau = 2. * pi;
	cout << "The smaller of pi and tau is: " << fmin(pi, tau) << '\n';
	return 0;
}
