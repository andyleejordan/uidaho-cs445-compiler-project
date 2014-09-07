#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clex.h"
#include "token.h"
#include "list.h"
#include "cgram.tab.h"

struct token *yytoken = NULL;
struct list *tokens = NULL;
struct list *filenames = NULL;

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
		list_push(tokens, (union data)yytoken);
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
	filenames = list_init();
	if (tokens == NULL || filenames == NULL)
		goto error_list_init;

	if (argc == 1) {
		list_push(filenames, (union data)"stdin");
		yyin = stdin;
		parse_file();
	} else {
		for (int i = 1; i < argc; ++i) {
			list_push(filenames, (union data)argv[i]);
			yyin = fopen(list_peek(filenames).filename, "r");
			if (yyin == NULL)
				goto error_fopen;
			parse_file();
		}
	}

	printf("Line/Filename    Token   Text -> Ival/Sval\n");
	printf("------------------------------------------\n");
	struct list_node *iter = list_head(tokens);
	while (!list_end(iter)) {
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

 error_fopen:
	perror("fopen(filename)");
	return EXIT_FAILURE;
}
