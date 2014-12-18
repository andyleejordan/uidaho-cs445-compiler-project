/* test code for classes */
#include <iostream>
using namespace std;

void nonmethod()
{
	return;
}

class TestClass {
private:
	int data;
public:
	TestClass();
	void setData(int);
	int getData(int);
	int moredata;
};

int main(int argc, char *argv[])
{
	nonmethod();

	TestClass myclass;

	cout << "Testing\n";
	myclass.moredata = 2;
	cout << myclass.moredata << '\n';
	myclass.setData(3);
	cout << myclass.getData(1) << '\n';

	TestClass *myaddress = &myclass;

	TestClass *mynew = new TestClass();

	cout << "More data starts at " << mynew->moredata << '\n';
	mynew->moredata = 3;
	cout << "And should now be 3: " << mynew->moredata << '\n';
	cout << "And data is " << mynew->getData(4) << '\n';

	delete mynew;

	return 0;
}

TestClass::TestClass()
{
	data = 42;
	moredata = 0;
}

void TestClass::setData(int i)
{
	data = i;
	cout << "Set data!\n";
}

int TestClass::getData(int i)
{
	return data + i;
}
