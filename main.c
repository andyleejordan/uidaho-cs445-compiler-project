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
void parse_file()
{
	while (true) {
		int temp = yylex();
		if (temp == 0) {
			return;
		} else if (temp == BADTOKEN) {
			fprintf(stderr, "Lexical error on line %d: %s\n", yytoken->lineno, yytoken->text);
			exit(1); /* required to exit with 1 on lexical error */
		} else if (temp < BEGTOKEN || temp > ENDTOKEN ) {
			fprintf(stderr, "Unkown return from yylex %d\n", temp);
			exit(EXIT_FAILURE);
		}
		tokenlist_append(yytoken, &tail);
	}
}

int main(int argc, char **argv)
{
	tokenlist_init(&head, &tail);

	filename = calloc(strlen("stdin")+1, sizeof(char));
	if (filename == NULL) {
		free(filename);
		fprintf(stderr, "Error reallocating memory for filename");
		exit(EXIT_FAILURE);
	}

	if (argc == 1) {
		strcpy(filename, "stdin");
		yyin = stdin;
		parse_file();
	} else {
		for (int i = 1; i < argc; ++i) {
			char *buffer = realloc(filename, sizeof(argv[i]));
			if (buffer) {
				filename = buffer;
				strcpy(filename, argv[i]);
			} else {
				free(filename);
				fprintf(stderr, "Error reallocating memory for filename");
				exit(EXIT_FAILURE);
			}
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
