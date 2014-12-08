int main(int argc, char *argv[])
{
	int myarray[11];
	for (int i = 0; i < (sizeof(myarray) / sizeof(myarray[0])); ++i) {
		myarray[i] = i;
	}

	return 0;
}
