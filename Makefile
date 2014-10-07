# binaries
BIN=120++
TESTS=test-list test-tree
CC=gcc
LEX=flex
YACC=bison
RM=rm -f

# compile options
CFLAGS=-g -Wall -std=gnu99
YFLAGS=-Wall --report=all

# archive options
GITREF=HEAD
PREFIX=$(BIN)

# targets
all: $(BIN) $(TESTS)

test: $(TESTS)

test-lex: $(BIN)
	./$(BIN) $(LEX_TESTS)

tar:
	git archive --format=tar --prefix=$(PREFIX)/ $(GITREF) > $(PREFIX).tar

TEMP_OBJECTS=lexer.h lex.yy.c parser.tab.h parser.tab.c parser.output
clean:
	$(RM) $(BIN) $(TESTS) $(OBJECTS) $(TEST_OBJECTS) $(TEMP_OBJECTS)

.PHONY: all test-lex tar clean

# source
SOURCES=main.c token.c list.c tree.c lex.yy.c parser.tab.c
OBJECTS=$(SOURCES:.c=.o)

$(BIN): $(OBJECTS)
	$(CC) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

main.c: lexer.h parser.tab.h

parser.tab.h parser.tab.c: parser.y
	$(YACC) $(YFLAGS) $<

lexer.h: lex.yy.c

lex.yy.c: lexer.l parser.tab.h
	$(LEX) $<

token.c: token.h

list.c: list.h

tree.c: tree.h

test/test.c: test/test.h

# tests
LEX_TESTS=test/lex/test.c test/lex/test.cpp

TEST_SOURCES=test/test.c list.c tree.c
TEST_OBJECTS=$(TEST_SOURCES:.c=.o)

test-list: test/list.c $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@
	./$@

test-tree: test/tree.c $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@
	./$@
