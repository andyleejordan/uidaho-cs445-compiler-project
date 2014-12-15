/* testing sizeof, array indexing, and assignments with pointers */
int main(int argc, char *argv[])
{
	int myarray[11];
	for (int i = 0; i < (sizeof(myarray) / sizeof(myarray[0])); ++i) {
		myarray[i] = i;
	}

	int anint = 4;
	myarray[3] = anint;

	int *myptr = &anint;
	*myptr = 2;

	int another;
	another = 3 * *myptr;
	another = *myptr;

	return 0;
}
