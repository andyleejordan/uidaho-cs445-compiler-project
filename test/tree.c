#include <stdio.h>
#include <string.h>

#include "test.h"
#include "../tree.h"
#include "../list.h"

void print_tree(struct tree *t)
{
	printf("%s ", t->data);
}

void test_size(struct tree *tree, size_t size)
{
	if (tree_size(tree) != size) {
		sprintf(buffer, "size should have been %lu", size);
		failure(buffer);
	}
}

void test_init(struct tree *tree, struct tree *parent, void *data)
{
	test_size(tree, 1);

	if (tree->parent != parent)
		failure("parent wasn't assigned");

	if (tree->data != data)
		failure("data wasn't assigned");

	if (!list_empty(tree->children))
		failure("new children list wasn't empty");
}

void test_initv()
{
	struct tree *tree = tree_initv(NULL, "root", 2, "foo", "bar");

	test_size(tree, 3);

	if (tree->parent != NULL)
		failure("parent wasn't NULL");

	if (!compare(tree->data, "root"))
		failure("data wasn't 'root'");

	struct tree *c1 = list_pop(tree->children);
	if (!compare(c1->data, "bar"))
		failure("last child wasn't 'bar'");

	struct tree *c2 = list_pop(tree->children);
	if (!compare(c2->data, "foo"))
		failure("first child wasn't 'foo'");
}

int main(int argc, char *argv[])
{
	running("tree");

	testing("initialization");
	char *a = strdup("+");
	struct tree *root = tree_init(NULL, a);
	test_init(root, NULL, a);
	test_size(root, 1);

	testing("push depth 1");
	char *b = strdup("1");
	struct tree *child1 = tree_push(root, b);
	test_init(child1, root, b);
	test_size(root, 2);
	char *c = strdup("*");
	struct tree *child2 = tree_push(root, c);
	test_init(child2, root, c);
	test_size(root, 3);

	testing("push depth 2");
	char *d = strdup("2");
	struct tree *child3 = tree_push(child2, d);
	test_init(child3, child2, d);
	char *e = strdup("3");
	struct tree *child4 = tree_push(child2, e);
	test_init(child4, child2, e);
	test_size(child2, 3);
	test_size(root, 5);

	testing("printing:");
	tree_preorder(root, &print_tree);
	printf("\n");

	tree_destroy(root, &free);

	testing("variadic push 2 args");
	test_initv();

	return status;
}
