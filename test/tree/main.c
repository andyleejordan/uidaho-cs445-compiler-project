#include <stdio.h>
#include <string.h>

#include "../../tree.h"
#include "../../list.h"

char *convert(char *data)
{
	return (char *)data;
}

int main(int argc, char *argv[])
{
	char *a = strdup("+");
	char *b = strdup("1");
	char *c = strdup("*");
	char *d = strdup("3");
	char *e = strdup("4");

	struct tree *root = tree_init(NULL, a);

	struct tree *childA = tree_push(root, b);
	struct tree *childB = tree_push(root, c);

	tree_push(childB, d);
	tree_push(childB, e);

	printf("Total size is %lu\n", tree_size(root));
	tree_print(root, (char *(*)(void *))&convert);
	printf("\n");

	tree_destroy(root, (void (*)(void *))&free);

	return 0;
}
