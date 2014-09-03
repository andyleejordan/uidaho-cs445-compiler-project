#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgram.tab.h"

int yylex();
extern FILE* yyin;
extern int yylineno;
extern char* yytext;

typedef struct token
{
  int category;
  int lineno;
  char* text;
  char* filename;
  int ival;
  char* sval;
} token_t;

typedef struct tokenlist
{
  struct token* data;
  struct tokenlist* next;
} tokenlist_t;

/* prepends token to list and returns pointer to new head of list */
tokenlist_t* tokenlist_prepend(token_t* tok, tokenlist_t* list)
{
  tokenlist_t* cur = malloc(sizeof(tokenlist_t));
  cur->data = tok;
  cur->next = list;
  return cur;
}

tokenlist_t* head = NULL;

/* TODO return yywrap value*/
int parse_file(char* filename)
  {
    while(1)
      {
	token_t* tok = malloc(sizeof(token_t));

	tok->category = yylex();
	tok->lineno = yylineno;
	tok->text = calloc(strlen(yytext)+1, sizeof(char));
	strcpy(tok->text, yytext);
	tok->filename = calloc(strlen(filename)+1, sizeof(char));
	strcpy(tok->filename, filename);

	if (tok->category == ICON) { tok->ival = atoi(yytext); }
	if (tok->category == STRING)
	  {
	    tok->sval = calloc(strlen(yytext)+1, sizeof(char));
	    strcpy(tok->sval, yytext);
	    /* TODO parse escaped characters */
	  }

#ifdef DEBUG
	printf("line %d - category %d - lexeme: %s\n", tok->lineno, tok->category, tok->text);
#endif

	/* TODO handle EOF better */
	if (tok->category == -1)
	  {
	    free(tok);
	    return -1;
	  }

	head = tokenlist_prepend(tok, head);
      }
  }

int main(int argc, char** argv)
{
  if (argc == 1)
    {
      yyin = stdin;
      parse_file("stdin");
    }
  else
    {
      for (int i = 1; i < argc; ++i)
	{
	  char* filename = argv[i];
	  yyin = fopen(filename, "r");
	  parse_file(filename);
	}
    }

  printf("Category\tText\tLineno\tFilename\tIval/Sval\n");
  tokenlist_t* cur = head;
  while (cur != NULL)
    {
      printf("%d\t%s\t%d\t%s",
	     cur->data->category,
	     cur->data->text,
	     cur->data->lineno,
	     cur->data->filename);
      if (cur->data->category == ICON)
	{ printf("\t%d", cur->data->ival); }
      else if (cur->data->category == STRING)
	{ printf("\t%s", cur->data->sval); }
      printf("\n");
      cur = cur->next;
    }

  /* TODO free memory */
  return 0;
}
