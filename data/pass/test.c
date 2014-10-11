#include "./test.h"

struct test
{
	/* struct test *next; */
	/* struct test *prev; */
	int data;
};

int main(int argc, char *argv[])
{
	test mytest;
	mytest.data = 42;

	test *mytestptr = malloc(sizeof(*mytestptr));
	mytestptr->data = 42;

	do {
		/* skip first argument */
		--argc;
		++argv;

		printf("%s\n", *argv); // print all arguments
	} while (argc > 0);

	// the /* c */ is a char
	char c = 'C';
	char newline = '\n';
	char backslash = '\\';
	char tick = '\'';

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
	/* struct test *tmp = malloc(sizeof(*tmp)); */

	/* tmp->data = (1 == 0) */
	/* 	? 1 */
	/* 	: 2; */

	/* free(tmp); */

	/* 512 bytes */
	char *loremipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque tincidunt id velit nec laoreet. Maecenas feugiat cursus finibus. Ut convallis volutpat urna, nec cursus nisl. Nullam malesuada sapien et maximus ultricies. Suspendisse ut justo dui. Nulla bibendum id turpis sed lobortis. Sed vitae turpis vitae arcu pulvinar aliquet. Nam a faucibus mi. Integer rutrum mattis nisl, facilisis pharetra lorem dapibus finibus. Aenean eget lacus non augue faucibus lobortis et a sapien. Duis eleifend maximus cras amet.";

	char *nullsucks = "A string with \0 i.e. escaped null (\0) a couple times";
}
