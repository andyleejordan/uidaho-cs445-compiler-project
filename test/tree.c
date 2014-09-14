#include <stdio.h>
#include <string.h>

#include "../tree.h"
#include "../list.h"

char *convert(char *data)
{
	return (char *)data;
}

void test_size(struct tree *tree, size_t size)
{
	if (tree_size(tree) != size)
		fprintf(stderr, "FAILURE tree size should have been %lu\n", size);
}

void test_init(struct tree *tree, struct tree *parent, void *data)
{
	test_size(tree, 1);

	if (tree->parent != parent)
		fprintf(stderr, "FAILURE tree's parent wasn't assigned\n");

	if (tree->data != data)
		fprintf(stderr, "FAILURE tree's data wasn't assigned\n");

	if (!list_empty(tree->children))
		fprintf(stderr, "FAILURE tree's new children list wasn't empty\n");
}

int main(int argc, char *argv[])
{
	printf("RUNNING tree tests\n");

	printf("TESTING tree initialization\n");
	char *a = strdup("+");
	struct tree *root = tree_init(NULL, a);
	test_init(root, NULL, a);
	test_size(root, 1);

	printf("TESTING tree push depth 1\n");
	char *b = strdup("1");
	struct tree *child1 = tree_push(root, b);
	test_init(child1, root, b);
	test_size(root, 2);
	char *c = strdup("*");
	struct tree *child2 = tree_push(root, c);
	test_init(child2, root, c);
	test_size(root, 3);

	printf("TESTING tree push depth 2\n");
	char *d = strdup("2");
	struct tree *child3 = tree_push(child2, d);
	test_init(child3, child2, d);
	char *e = strdup("3");
	struct tree *child4 = tree_push(child2, e);
	test_init(child4, child2, e);
	test_size(child2, 3);
	test_size(root, 5);

	printf("TESTING tree printing: ");
	tree_print(root, (char *(*)(void *))&convert);
	printf("\n");

	tree_destroy(root, &free);

	return 0;
}
