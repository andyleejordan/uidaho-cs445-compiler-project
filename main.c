#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "list.h"
#include "cgram.tab.h"

int yylex();
extern FILE *yyin;

struct token *yytoken = NULL;
char *filename = NULL;

struct list *tokens = NULL;

/* TODO return yywrap value*/
void parse_file()
{
	int category;
	while (true) {
		/* TODO check yywrap */
		category = yylex();
		if (category == 0)
			break;
		else if (category == BADTOKEN)
			goto error_badtoken;
		else if (category < BEGTOKEN || category > ENDTOKEN )
			goto error_unknown_token;
		if (tokens == NULL) fprintf(stderr, "WTF\n");
		list_append(tokens, (union data)yytoken);
	}
	return;

 error_badtoken: {
		fprintf(stderr, "Lexical error on line %d: %s\n", yytoken->lineno, yytoken->text);
		exit(1); /* required to exit with 1 on lexical error */
	}

 error_unknown_token: {
		fprintf(stderr, "Unkown category from yylex %d\n", category);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	tokens = list_init();
	if (tokens == NULL)
		goto error_list_init;

	filename = calloc(strlen("stdin")+1, sizeof(char));
	if (filename == NULL)
		goto error_filename;

	if (argc == 1) {
		strcpy(filename, "stdin");
		yyin = stdin;
		parse_file();
	} else {
		for (int i = 1; i < argc; ++i) {
			char *buffer = realloc(filename, sizeof(argv[i]));
			if (buffer == NULL)
				goto error_filename;
			filename = buffer;
			strcpy(filename, argv[i]);
			yyin = fopen(filename, "r");
			if (yyin == NULL)
				goto error_fopen;
			parse_file();
		}
	}

	printf("Line/Filename    Token   Text -> Ival/Sval\n");
	printf("------------------------------------------\n");
	struct list_node *iter = tokens->sentinel->next;
	while (!iter->data.sentinel) {
		printf("%-5d%-12s%-8d%s ",
		       iter->data.token->lineno,
		       iter->data.token->filename,
		       iter->data.token->category,
		       iter->data.token->text);

		if (iter->data.token->category == ICON)
			printf("-> %d", iter->data.token->ival);
		else if (iter->data.token->category == CCON)
			printf("-> %c", iter->data.token->ival);
		else if (iter->data.token->category == SCON)
			printf("-> %s", iter->data.token->sval);

		printf("\n");
		iter = iter->next;
	}

	return EXIT_SUCCESS;

 error_list_init:
	perror("list_init()");
	return EXIT_FAILURE;

 error_filename:
	free(filename);
	perror("filename");
	return EXIT_FAILURE;

 error_fopen:
	perror("fopen(filename)");
	return EXIT_FAILURE;
}
