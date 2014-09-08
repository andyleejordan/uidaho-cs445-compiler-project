#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "./test.h"

struct test
{
	struct test *next;
	struct test *prev;
	int data;
};

int main(int argc, char *argv[])
{
	do {
		/* skip first argument */
		--argc;
		++argv;

		printf("%s\n", *argv); // print all arguments
	} while (argc > 0);

	// the /* c */ is a char
	const char c = 'C';
	const char newline = '\n';
	const char backslash = '\\';
	const char tick = '\'';

	switch (c) {
	case 'C': {
		printf("char 'c' was %c\n", c);
		break;
	}
	default:
		printf("Switch fell through\n");
	}

	/*************************
	 * use "malloc" -> "free"
	 *************************/
	struct test *tmp = (struct test*)malloc(sizeof(*tmp));

	tmp->data = (1 == true)
		? 1
		: 2;

	free(tmp);
}
