/*
 * test.cpp - Valid C++ code for testing parser.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#include <iostream>
#include <string>

using namespace std;

class TestClass {
public:
	TestClass();
	TestClass(string data);
	string data;
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

	TestClass myclass;

	bool is = false;
	char character = 'A';
	short quick = 2;
	long big = 123234;
	double pi = 3.14159;
	double zero = .0000000001;
	float one = 1.;

	string empty = "";

	TestClass *tmpclass = new TestClass;
	cout << tmpclass->data << endl;
	delete tmpclass;

	return 0;
}

TestClass::TestClass(): data("Hello world!\n")
{}

TestClass::TestClass(string data): data(data)
{
	cout << "TestClass \"ctor\" received data!" << endl;
}
