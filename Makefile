# Makfile - Creates the 120++ compiler.
# Copyright (C) 2014 Andrew Schwartzmeyer
# This file released under the AGPLv3.

SHELL = /bin/sh

# generated executables
BIN = 120++
TESTS = test-list test-tree

# dependencies
CC = gcc
FLEX = flex
BISON = bison

# flags
CDEBUG = -g
CFLAGS = -O -Wall -std=gnu99
LDFLAGS = -g
FLEXFLAGS =
BISONFLAGS = -Wall

# git archive option
GITREF = $(shell git tag | tail -n 1)

# local machine specifics
-include local.mk

# files
SRCS = main.c token.c list.c tree.c lex.yy.c parser.tab.c
OBJS = $(SRCS:.c=.o)
TEST_SRCS = test/test.c list.c tree.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_DATA = test/lex/test.c test/lex/test.cpp

# targets
.PHONY: all test smoke dist clean distclean

all: $(BIN) test

test: $(TESTS)
	./test-list
	./test-tree

smoke: $(BIN)
	./$(BIN) $(TEST_DATA)

TAGS: $(SRCS)
	etags $(SRCS)
dist:
	git archive --format=tar --prefix=$(GITREF)/ $(GITREF) > $(GITREF).tar

clean:
	rm -f $(BIN) $(TESTS) *.o lexer.h lex.yy.c parser.tab.h parser.tab.c

distclean: clean
	rm -f TAGS *.tar

# sources
$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) $(CDEBUG) -c $< -o $@

main.c: lexer.h parser.tab.h

parser.tab.h parser.tab.c: parser.y
	$(BISON) $(BISONFLAGS) $<

lexer.h: lex.yy.c

lex.yy.c: lexer.l parser.tab.h
	$(FLEX) $(FLEXFLAGS) $<

token.c: token.h

list.c: list.h

tree.c: tree.h

test/test.c: test/test.h

test-list: test/list.c $(TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

test-tree: test/tree.c $(TEST_OBJS)
	$(CC) $(CFLAGS) $^ -o $@
