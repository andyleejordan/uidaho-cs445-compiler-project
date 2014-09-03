struct token
{
  int category;   /* the integer code returned by yylex */
  char *text;     /* the actual string (lexeme) matched */
  int lineno;     /* the line number on which the token occurs */
  char *filename; /* the source file in which the token occurs */
  int ival;       /* if you had an integer constant, store its value here */
  int *sval;      /* if you had a string constant, malloc space and store */
};               /*    the string (less quotes and after escapes) here */

struct tokenlist
{
  struct token *t;
  struct tokenlist *next;
};

int main(int argc, char** argv)
{
  return 0;
}
