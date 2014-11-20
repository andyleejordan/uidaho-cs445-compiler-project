#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class DefaultThing {
public:
	double data;
	void myf(double blah);
};

class AClass {
public:
	AClass(int, double);
	double data[2];
	void classtest(char *, int);
	int w, x, *y;
private:
	int *hidingstuff();
};

void DefaultThing::myf(double blah)
{
}

AClass::AClass(int z, double d)
{
	w = z;
	x = z;
	y = new int;
	*y = z;
	delete y;
	data[0] = d;
	data[1] = d;
}


void AClass::classtest(char *s, int)
{
	return;
}

int double_array[3];
int a, *c, e;
double clkj;
double *ads;
char alkj[3];

int *foobar(int *asdf, AClass gadget);
int bar(int, double[]);

AClass *foo()
{
	int z = 2;
	AClass *someclass = new AClass(z, 23.1);
	someclass->x = 0;

	return someclass;
}

int main()
{
	string mystring = "hello";
	cout << 2 << 'd' << "foobar" << mystring << 3.14 << endl;
	for (int i = 0; i < 3; ++i) {
		continue;
	}

	double myfloats[2] = {1.2, 3.2};

	int lkj = bar(bar(2, myfloats), myfloats); //pass

	DefaultThing notapointer;
	notapointer.data = 2.8;

	int x = 1, y = 2, z = 3;

	undecladfadf = 4;

	// z = (x < y) ? 0 : 1;

	if (y > x)
		y = x;

	x = y + z;
	lkj = (z * y) / 1;

	if (z || !y)
		z = y;
	else if (z != y)
		y += z;
	else if (z == y)
		++z;
	else
		--z;

	switch (y) {
	case 0: break;
	case 1: break;
	}

	AClass *thingptr = foo();
	delete thingptr;

	lkj--;
	++lkj;

	foobar(&lkj, *thingptr);

	return 0;
}

int *foobar(int *asdf, AClass gadget)
{
	gadget.w = 2;

	int *integer = new int;
	*integer = *asdf;
	return integer;
}

int bar(int h, double nums[])
{
	nums[0] = 3.14;

	return h;
}
