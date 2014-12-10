#include <stdio.h>
#include <string.h>

#include "test.h"
#include "tree.h"
#include "list.h"

#define P(name, ...) tree_new_group(NULL, #name, NULL, &delete_tree, __VA_ARGS__)

bool print_tree(struct tree *t, int d);
void delete_tree(void *data, bool leaf);

void test_size(struct tree *tree, size_t size);
void test_new(struct tree *tree, struct tree *parent, void *data);
void test_new_group(struct tree *t);

int main()
{
	running("tree");

	testing("new");
	char *a = strdup("+");
	struct tree *root = tree_new(NULL, a, NULL, &delete_tree);
	test_new(root, NULL, a);
	test_size(root, 1);

	testing("push back depth 1");
	char *b = strdup("1");
	struct tree *child1 = tree_push_back(root, b);
	test_new(child1, root, b);
	test_size(root, 2);

	testing("push front depth 1");
	char *c = strdup("*");
	struct tree *child2 = tree_push_front(root, c);
	test_new(child2, root, c);
	test_size(root, 3);

	testing("push back depth 2");
	char *d = strdup("2");
	struct tree *child3 = tree_push_back(child2, d);
	test_new(child3, child2, d);
	char *e = strdup("3");

	testing("push front depth 2");
	struct tree *child4 = tree_push_front(child2, e);
	test_new(child4, child2, e);
	test_size(child2, 3);
	test_size(root, 5);

	testing("printing:");
	tree_traverse(root, 0, &print_tree, NULL, NULL);
	if (TREE_DEBUG)
		printf("\n");

	tree_free(root);

	testing("variadic push back 2 args");
	struct tree *v = tree_new_group(NULL, "root", NULL, &delete_tree, 2,
	                                tree_new(NULL, "foo", NULL, &delete_tree),
	                                tree_new(NULL, "bar", NULL, &delete_tree));
	test_new_group(v);

	testing("macro P push back 2 args");
	struct tree *p = P(root, 2,
	                   tree_new(NULL, "foo", NULL, &delete_tree),
	                   tree_new(NULL, "bar", NULL, &delete_tree));
	test_new_group(p);

	return status;
}

void test_size(struct tree *tree, size_t size)
{
	if (tree_size(tree) != size)
		failure("size should have been %zu", size);
}

void test_new(struct tree *tree, struct tree *parent, void *data)
{
	test_size(tree, 1);

	if (tree->parent != parent)
		failure("parent wasn't assigned");

	if (tree->data != data)
		failure("data wasn't assigned");

	if (!list_empty(tree->children))
		failure("new children list wasn't empty");
}

void test_new_group(struct tree *t)
{
	test_size(t, 3);

	if (t->parent)
		failure("parent wasn't NULL");

	if (!compare(t->data, "root"))
		failure("data wasn't 'root'");

	struct tree *c1 = list_pop_back(t->children);
	if (!compare(c1->data, "bar"))
		failure("last child wasn't 'bar'");

	struct tree *c2 = list_pop_back(t->children);
	if (!compare(c2->data, "foo"))
		failure("first child wasn't 'foo'");
}

bool print_tree(struct tree *t, int d)
{
	if (TREE_DEBUG)
		printf("%*s\n", d*2, (char *)t->data);
	return true;
}

void delete_tree(void *data, bool leaf)
{
	free(data);
}
