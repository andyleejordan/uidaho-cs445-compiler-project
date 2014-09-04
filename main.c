#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "cgram.tab.h"

int yylex();
extern FILE* yyin;

token_t* yytoken = NULL;
char* filename = NULL;
tokenlist_t* head = NULL;

/* TODO return yywrap value*/
int parse_file()
  {
    while(1)
      {
	int category = yylex();
	if (category < 0) { return category; }
	head = tokenlist_prepend(yytoken, head);
      }
  }

int main(int argc, char** argv)
{
  if (argc == 1)
    {
      filename = "stdin";
      yyin = stdin;
      parse_file();
    }
  else
    {
      for (int i = 1; i < argc; ++i)
	{
	  filename = argv[i];
	  yyin = fopen(filename, "r");
	  parse_file();
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
