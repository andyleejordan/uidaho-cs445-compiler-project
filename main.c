#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "clex.h"
#include "token.h"
#include "list.h"
#include "cgram.tab.h"

struct token *yytoken = NULL;
struct list *tokens = NULL;
struct list *filenames = NULL;

char *current_filename()
{
	char *filename = list_peek(filenames).filename;
	if (filename == NULL)
		return NULL;
	char *copy = calloc(strlen(filename)+1, sizeof(char));
	if (copy == NULL)
		goto error;
	strcpy(copy, filename);
	return copy;

 error:
	perror("current_filename()");
	exit(EXIT_FAILURE);
}

/* start the parser via yylex; error on relevant tokens; add good
   tokens to token list */
void parse_files()
{
	int category;
	while (true) {
		category = yylex();
		if (category == 0)
			break;
		else if (category < BEGTOKEN || category > ENDTOKEN )
			goto error_unknown_return_value;
		list_push(tokens, (union data)yytoken);
	}
	yylex_destroy();
	return;

 error_unknown_return_value:
	fprintf(stderr, "Unkown return value from yylex %d\n", category);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	tokens = list_init();
	filenames = list_init();
	if (tokens == NULL || filenames == NULL)
		goto error_list_init;

	if (argc == 1) {
		/* not 'char filename[] = "stdin"' because list_destroy */
		char *filename = malloc(sizeof("stdin"));
		if (filename == NULL)
			goto error_alloc;
		strcpy(filename, "stdin");

		list_push(filenames, (union data)filename);
		yyin = stdin;
		yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
	} else {
		for (int i = 1; i < argc; ++i) {
			/* get real path for argument */
			char *filename = realpath(argv[i], NULL);
			if (filename == NULL)
				goto error_realpath;

			/* open file and push filename and buffer */
			list_push(filenames, (union data)filename);
			yyin = fopen(filename, "r");
			if (yyin == NULL)
				goto error_fopen;
			yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
		}
		/* start is a special case where we need to chdir to
		   the directory of the first file to be parsed; all
		   other files are chdir'ed to after the previous has
		   been popped */
		char *filename = current_filename();
		if (filename)
			chdir(dirname(filename));
	}

	parse_files();

	printf("Line/Filename    Token   Text -> Ival/Sval\n");
	printf("------------------------------------------\n");
	struct list_node *iter = list_head(tokens);
	while (!list_end(iter)) {
		printf("%-5d%-12s%-8d%s ",
		       iter->data.token->lineno,
		       basename(iter->data.token->filename),
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

	list_destroy(tokens, (void (*)(union data))&token_free);
	list_destroy(filenames, (void (*)(union data))&free);

	return EXIT_SUCCESS;

 error_alloc: {
		perror("main: memory re/allocation()");
		return EXIT_FAILURE;
	}

 error_realpath: {
		perror("main: realpath()");
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
