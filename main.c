#include <stdbool.h>
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
struct tokenlist *tail = NULL;

/* TODO return yywrap value*/
int parse_file()
{
	while (true) {
		int tmp = yylex();
		if (tmp == 0) {
			return 0;
		} else if (tmp < 0) {
			fprintf(stderr, "yylex returned %d on line %d\n", tmp, yytoken->lineno);
			exit(EXIT_FAILURE);
		}
		tokenlist_append(yytoken, &tail);
	}
}

int main(int argc, char **argv)
{
	tokenlist_init(&head, &tail);

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

	printf("Line/Filename    Token   Text -> Ival/Sval\n");
	printf("------------------------------------------\n");
	struct tokenlist *tmp = head->next;
	while (tmp->data != NULL) {
		printf("%-5d%-12s%-8d%s ",
		       tmp->data->lineno,
		       tmp->data->filename,
		       tmp->data->category,
		       tmp->data->text);

		if (tmp->data->category == ICON)
			printf("-> %d", tmp->data->ival);
		else if (tmp->data->category == CCON)
			printf("-> %c", tmp->data->ival);
		else if (tmp->data->category == SCON)
			printf("-> %s", tmp->data->sval);

		printf("\n");
		tmp = tmp->next;
	}

	/* TODO free memory */
	return EXIT_SUCCESS;
}
