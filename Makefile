# Makfile - Creates the 120++ compiler.
# Copyright (C) 2014 Andrew Schwartzmeyer
# This file released under the AGPLv3.

SHELL = /bin/sh

# generated executables
BIN = 120
TESTS = test-list test-tree test-hasht

# dependencies
CC = gcc
FLEX = flex
BISON = bison

# flags
CDEBUG = -g
CFLAGS = -O -Wall -Werror -std=gnu99
LDFLAGS = -g
FLEXFLAGS =
BISONFLAGS = -Wall -Werror

# git archive option
GITREF = $(shell git tag | tail -n 1)

# local machine specifics
-include local.mk

# files
SRCS = main.c symbol.c token.c rules.c \
	list.c tree.c hasht.c lookup3.c \
	lex.yy.c parser.tab.c
OBJS = $(SRCS:.c=.o)
TEST_DATA = data/pass/test.c data/pass/test.cpp

# targets
.PHONY: all test smoke dist clean distclean

all: $(BIN) test

test: $(TESTS)
	./test-list
	./test-tree
	./test-hasht

smoke: all
	./$(BIN) $(TEST_DATA)

TAGS: $(SRCS)
	etags $(SRCS)
dist:
	git archive --format=tar $(GITREF) > $(GITREF).tar

clean:
	rm -f $(BIN) $(TESTS) *.o lexer.h lex.yy.c parser.tab.h parser.tab.c

distclean: clean
	rm -f TAGS *.tar

# sources
$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) $(CDEBUG) -o $@ -c $<

main.c: token.h symbol.h list.h tree.h hasht.h lexer.h

parser.tab.h parser.tab.c: parser.y
	$(BISON) $(BISONFLAGS) $<

parser.y: token.h list.h tree.h rules.h

lexer.h: lex.yy.c

lex.yy.c: lexer.l
	$(FLEX) $(FLEXFLAGS) $<

lexer.l: token.h list.h tree.h hasht.h parser.tab.h

token.o: token.h

symbol.o: symbol.h

list.o: list.h

tree.o: tree.h list.h

test.o: test.h

hasht.o: hasht.h lookup3.o

BUILD_TEST = $(CC) $(CFLAGS) $(CDEBUG) -o $@ $^
test-list: test_list.o list.o test.o
	$(BUILD_TEST)

test-tree: test_tree.o tree.o list.o test.o
	$(BUILD_TEST)

test-hasht: test_hasht.o hasht.o lookup3.o test.o
	$(BUILD_TEST)
