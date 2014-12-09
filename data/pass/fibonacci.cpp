/* for testing multiple parameters to functions */
int paramzzz(char x, int y, int z);
int fibonacci(int n);

int main(int argc, char *argv[])
{
	int c = 0;

	/* test some loops while we're at it */
	while (c < 4) {
		c = c + 2;
		if (c > 1)
			continue;
		c = c - 1;
	}

	for (int i = 0; i < 32; ++i)
		c++;

	int fib;
	do {
		fib = fibonacci(c % 16);
		if (fib < 0) {
			break;
		} else {
			fib = fibonacci(8);
		}
	} while (0);

	return paramzzz('c', fib, 2);
}

int fibonacci(int n)
{
	if (n == 0)
		return 0;
	else if (n == 1)
		return 1;
	else
		return fibonacci(n - 1) + fibonacci(n - 2);
}

int paramzzz(char x, int y, int z)
{
	return 2;
}
