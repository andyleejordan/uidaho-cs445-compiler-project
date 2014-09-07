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

void parse_files()
{
	int category;
	while (true) {
		category = yylex();
		if (category == 0)
			break;
		else if (category == BADTOKEN)
			goto error_badtoken;
		else if (category < BEGTOKEN || category > ENDTOKEN )
			goto error_unknown_return_value;
		list_push(tokens, (union data)yytoken);
	}
	return;

 error_badtoken: {
		fprintf(stderr, "Lexical error in file \"%s\" on line %d: %s\n",
		        yytoken->filename, yytoken->lineno, yytoken->text);
		exit(1); /* required to exit with 1 on lexical error */
	}

 error_unknown_return_value: {
		fprintf(stderr, "Unkown return value from yylex %d\n", category);
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
		char *filename = calloc(strlen("stdin"), sizeof(char));
		if (filename == NULL)
			goto error_calloc;
		strcpy(filename, "stdin");

		list_push(filenames, (union data)filename);
		yyin = stdin;
		yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
	} else {
		for (int i = 1; i < argc; ++i) {
			char *filename = calloc(strlen(argv[i]), sizeof(char));
			if (filename == NULL)
				goto error_calloc;
			strcpy(filename, argv[i]);

			list_push(filenames, (union data)filename);
			yyin = fopen(filename, "r");
			if (yyin == NULL)
				goto error_fopen;
			yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
		}
	}

	parse_files();

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

	while (!list_empty(tokens)) {
		union data d = list_pop(tokens);
		token_free(d.token);
	}

	list_destroy(filenames);

	return EXIT_SUCCESS;

 error_calloc: {
		perror("main: calloc()");
		return EXIT_FAILURE;
	}

 error_list_init: {
		perror("main: list_init()");
		return EXIT_FAILURE;
	}

 error_fopen: {
		perror("main: fopen(filename)");
		return EXIT_FAILURE;
	}
}
