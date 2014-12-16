/* test code for classes */

void nonmethod()
{
	return;
}

class TestClass {
private:
	int data;
public:
	TestClass();
	int getData(int);
	int moredata;
};

int main(int argc, char *argv[])
{
	nonmethod();

	TestClass myclass;

	myclass.moredata = 2;
	myclass.getData(1);

	TestClass *myaddress = &myclass;

	TestClass *mynew = new TestClass();

	mynew->moredata = 3;
	mynew->getData(4);

	delete mynew;

	return 0;
}

TestClass::TestClass()
{
	data = 42;
}

int TestClass::getData(int i)
{
	return data + i;
}
