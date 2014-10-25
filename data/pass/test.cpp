/*
 * test.cpp - Valid C++ code for testing parser.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class TestClass {
public:
	TestClass(int, double);
	void writeMethod(ofstream file, char *message);
	string data;
private:
	double *hiddenData();
};

int main(int argc, char *argv[])
{
	/*
	 * This
	 * is
	 * a                              // super difficult
	 * to parse
	 * test
	 * comment
	 */

	TestClass *myclass = new TestClass(2, 23.2);

	bool is = false;
	char character = 'A';
	short quick = 2;
	long big = 123234;
	double pi = 3.14159;
	double zero = .0000000001;
	float one = 1.;

	string empty = "";

	delete myclass;

	return 0;
}

void TestClass::writeMethod(ofstream file, char *message)
{
	// file << message;
	bool empty = false;
	return;
}

double *TestClass::hiddenData()
{
	for (int i = 0; i < 2; ++i)
		bool ha = true;

	double *pi = new double;

	return pi;
}

TestClass::TestClass(int lkaja, double oiud): data("Hello world!\n")
{
	bool definingctor = true;
}
