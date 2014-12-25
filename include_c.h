/*
 * include_c.h - Prototypes for commonly used C functions.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3 license.
 */

/* stdlib.h */
void *malloc(int);
void *calloc(int, int);
void *realloc(void *, int);
void free(void *);
void abort();
void exit(int);
int system(char *);
char *getenv(char *);

/* string.h */
int isalnum(int);
int isalpha(int);
int islower(int);
int isupper(int);
int isdigit(int);
int isspace(int);
int isblank(int);
int ispunct(int);
int tolower(int);
int toupper(int);
double atof(char *);
int atoi(char *);
char *strcpy(char *, char *);
char *strncpy(char *, char *, int);
char *strcat(char *, char *);
char *strncat(char *, char *, int);
int strlen(char *);
int strcmp(char *, char *);
int strncmp(char *, char *, int);
char *strchr(char *, int);
char *strrchr(char *, int);
char *strstr(char *, char *);
char *strtok(char *, char *);
char *strdup(char *);
char *strndup(char *, int);
void *memcmp(void *, void *, int);
void *memcpy(void *, void *, int);
void *memmove(void *, void *, int);

/* time.h */
int time(int *);
