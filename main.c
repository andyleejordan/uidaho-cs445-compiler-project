#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "cgram.tab.h"

int yylex();
extern FILE *yyin;

struct token *yytoken = NULL;
char *filename = NULL;
struct tokenlist *head = NULL;

/* TODO return yywrap value*/
int parse_file()
{
	while (1) {
		int category = yylex();
		if (category < 0)
			return category;
		head = tokenlist_prepend(yytoken, head);
	}
}

int main(int argc, char **argv)
{
	if (argc == 1) {
		filename = "stdin";
		yyin = stdin;
		parse_file();
	} else {
		for (int i = 1; i < argc; ++i) {
			filename = argv[i];
			yyin = fopen(filename, "r");
			parse_file();
		}
	}

	printf("Category\tText\tLineno\tFilename\tIval/Sval\n");
	struct tokenlist *tmp = head;
	while (tmp != NULL) {
		printf("%d\t%s\t%d\t%s",
		       tmp->data->category,
		       tmp->data->text,
		       tmp->data->lineno,
		       tmp->data->filename);

		if (tmp->data->category == ICON)
			printf("\t%d", tmp->data->ival);
		else if (tmp->data->category == STRING)
			printf("\t%s", tmp->data->sval);

		printf("\n");
		tmp = tmp->next;
	}

	/* TODO free memory */
	return 0;
}
