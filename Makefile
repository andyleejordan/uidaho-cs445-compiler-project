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
	$(RM) $(BIN) $(TESTS) $(OBJECTS) lex.yy.c clex.h

# source
SOURCES=main.c list.c token.c lex.yy.c
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

# tests
LEX_TESTS=test/lex/test.c test/lex/test.cpp

test-list: list.c test/list.c
	$(CC) $(CFLAGS) $^ -o $@
	./$@

test-tree: tree.c list.c test/tree.c
	$(CC) $(CFLAGS) $^ -o $@
	./$@
