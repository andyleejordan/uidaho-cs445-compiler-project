# binaries
BIN=120++
TESTS=test-list test-tree
CC=gcc
LEX=flex
RM=rm -f

# compile options
CFLAGS=-g -Wall -std=gnu99

# targets
all: $(BIN) $(TESTS)

test: $(TESTS)

test-lex: $(BIN)
	./$(BIN) $(LEX_TESTS)

clean:
	$(RM) $(BIN) $(TESTS) $(OBJECTS) $(TEST_OBJECTS) lex.yy.c clex.h

# source
SOURCES=main.c token.c list.c tree.c lex.yy.c
OBJECTS=$(SOURCES:.c=.o)

$(BIN): $(OBJECTS)
	$(CC) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

main.c: clex.h

clex.h: lex.yy.c

lex.yy.c: clex.l cgram.tab.h
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
