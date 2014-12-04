int fibonacci(int n);

int main(int argc, char *argv[])
{
	int i = 2;
	if (i == 2)
		i = i * 2;
	fibonacci(i % 8);

	return 0;
}

int fibonacci(int n)
{
	if (n == 0) {
		return 0;
	} else if (n == 1) {
		return 1;
	} else {
		return fibonacci(n - 1) + fibonacci(n - 2);
	}
}
