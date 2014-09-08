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

	TestClass *tmpclass = new TestClass;
	cout << tmpclass->data << endl;
	delete tmpclass;

	return 0;
}

TestClass::TestClass(): data("Hello world!")
{}

TestClass::TestClass(string data): data(data)
{
	cout << "TestClass \"ctor\" received data!" << endl;
}
