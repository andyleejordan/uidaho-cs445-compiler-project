int fibonacci(int n);

int main(int argc, char *argv[])
{
	int c = 0;

	while (c < 4) {
		c = c + 2;
	}

	for (int i = 0; i < 32; ++i)
		c++;

	do {
		fibonacci(c % 16);
	} while (0);

	return 0;
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
