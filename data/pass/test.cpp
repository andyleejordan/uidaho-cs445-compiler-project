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

#include "./test.h"

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

	test mytest;
	mytest.data = 42;

	TestClass *myclass = new TestClass(2, 23.2);

	// the /* c */ is a char
	char c = 'C';
	char newline = '\n';
	char backslash = '\\';
	char tick = '\'';
	char *loremipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque tincidunt id velit nec laoreet. Maecenas feugiat cursus finibus. Ut convallis volutpat urna, nec cursus nisl. Nullam malesuada sapien et maximus ultricies. Suspendisse ut justo dui. Nulla bibendum id turpis sed lobortis. Sed vitae turpis vitae arcu pulvinar aliquet. Nam a faucibus mi. Integer rutrum mattis nisl, facilisis pharetra lorem dapibus finibus. Aenean eget lacus non augue faucibus lobortis et a sapien. Duis eleifend maximus cras amet.";
	char *nullsucks = "A string with \0 i.e. escaped null (\0) a couple times";

	switch (c) {
	case 'C': {
		break;
	}
	default:
		break;
	}

	bool is = false;
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
