/* test code for classes */

class TestClass {
private:
	int data;
public:
	TestClass();
	int getData(int);
};

int main(int argc, char *argv[])
{
	TestClass myclass;

	TestClass *myclassptr = &myclass;

	return myclassptr->getData(1);
}

TestClass::TestClass()
{
	data = 42;
}

int TestClass::getData(int i)
{
	return data + i;
}
